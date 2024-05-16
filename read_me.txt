1.	组[MAIN]中包含两项全局设置：
CFG_NAME：此配置文件的名称，此名称将展示在软件界面的配置列表中，此外，若导入了两个重名的配置文件，后导入的将自动在末尾附加‘c’字符。

LIB_PATH：此项指出库文件的位置，路径从软件可执行文件所在文件夹向下查找。

2.	组[CFG]中包含一些配置信息
CORE_COUNT：此项指明当前配置所含内核的数量，此项也将体现在软件界面的配置列表中。

CORE0：此项指示CORE0所对应的ICEPick的tap号。（注：CORE_COUNT配置为多少，此处就应有多少项）

CURRENT_CORE：指示当前所用的内核（例：CURRENT_CORE=2，此时所用的内核为CORE2）。

WITH_ICEPICK：指明当前cfg是否有ICEPick层。若无ICEPick，则不会进行切Tap的操作。

USE_DMA：指示当前cfg文件是否使用DMA做数据搬移，若配置为false，则在生成码时将采用写数据写地址的方式进行，同时后边配置的DMA_START_SEQ及DMA_STOP_SEQ将不会使用。

DMA_IN_ONE_PROCESS：此项表明DMA分段进行（false）还是一次完成（true）（若一次进行，则地址不连续的地方将填0）。

RAM_ADDRESS_MODE：此项用于指示地址模式，常用的为8bit模式，还有16bit模式（2812/28035/28377为此模式），16bit模式下，所有的地址将进行/2操作。

PC_OFFSET：此项用于指明pc地址在coff文件中的偏移量，用于从coff文件的可选头中读取pc地址（66AK/6657的偏移量为24，2812/28377/28035/5510/169/240为38）

ENDIAN：此项表明大小端（little/big）（5510/240为大端，66x/28x为小端）

CODE_FOR_ATE/ CODE_FOR_TEST/ CODE_FOR_EMU：表明默认的生成码格式，此三项分别为机测码/实装码/仿真码

CODE_FOR_EMU_WITH_TCK：此项用于指明生成的仿真码是否含有TCK的变化（true：TCK将有0/1跳变，false：TCK始终为1）

#解析器配置
PARSER_NAME：此项指明用于解析coff文件的解析器名称，主要用于支持TI的解析器工具（此项也可配置成default，此时使用的时软件自带的解析器）

PARSER_SRC_CMD：指明源文件前的指令码（PARSER_NAME配置为default时无效）

PARSER_DEST_CMD：指明目标文件前的指令码（PARSER_NAME配置为default时无效）
注：整体的指令格式为：PARSER_NAME PARSER_SRC_CMD 源文件 PARSER_DEST_CMD 目标文件(out.txt)，例：hex6x --ti_txt E:\0jtagtoolTest\66akAteUartDspCase -o out.txt


3.	组[CODE]包含生成码流程等的配置
SEQ_COUNT：指明整个流程的数量（每个流程对应一个csv或者xlx/xlsx文件），需注意流程中需要有一个DATA流程，DATA用于指明于何时下发数据（若USE_DMA配置为true，则DMA_START_SEQ和DMA_STOP_SEQ将自动附加在DATA段中需要使用DMA的地方，因此在整个流程中不需要体现DMA的指令）
例：SEQ_COUNT=4
SEQ0=connect.csv
SEQ1=DATA
SEQ2=movePc.csv
SEQ3=run.csv

DMA_START_SEQ/ DMA_STOP_SEQ：指明DMA起始及DMA结束时的指令文件。

PC_MOVE_SEQ：指出整体流程中哪一段为移动PC的指令文件，用于pc地址的填入。（此流程需要在整体流程中体现，即此项配置会去寻找此指令文件在整体流程中位置，并进行操作）

DMA_REPLACE_COUNT：指明在DMA_START_SEQ指令段中，load地址需要在多少个地方进行替换。

DMA_ADDRESS_REPLACE_SIGN：指明load地址替换的标志，可以使用数字或自定义特殊字符（如@ADD或者55AA之类，不要包含0x，因为0x默认为十六进制数字前缀，将在匹配时被删除），此处配置的标志需与命令文件中需要被替换位置的字符一致。（注，若标志字符与需要替换的地址长度不一致时，标志字符在指令中的最低位的位置（比特位）须与替换后的值的最低位位置相同，高位则填零。例：标志字符为@A，替换的地址为0x12345A5A，地址替换后的命令为0xFEF12345A5A，则命令文件中的对应指令应为0xFEF000000@A）

DMA_REPLACE_BIT_LOCAL0：指明第一次替换从load地址的哪一位开始。(DMA_REPLACE_BIT_LOCAL1/ DMA_REPLACE_BIT_LOCAL2… 同)

DMA_REPLACE_BIT_COUNT0：指明第一次替换的bit数。（DMA_REPLACE_BIT_COUNT1/ DMA_REPLACE_BIT_COUNT2…同）

PC_REPLACE_COUNT/PC_ADDRESS_REPLACE_SIGN/PC_REPLACE_BIT_LOCAL0/PC_REPLACE_BIT_COUNT0：此处配置用于指导PC地址的替换，规则同DMA一致。

4.    组[CMD]包含指令相关的信息
CORE_CMD_LENGTH：指明内核指令的长度，用于DATA段流程中数据指令等的下发。

DMA_DATA_LENGTH：指明DATA段流程中，一条内核指令中包含的数据位数。

DMA_CMD_HEAD：指明DATA段中，通过DMA下发指令时的命令头（例：66AK为0xFFC00000000）

REG_WRITE_ADDRESS_HEAD：指明DATA段流程中不通过DMA下发数据时的写地址指令头。

REG_WRITE_DATA_HEAD：指明DATA段流程中不通过DMA下发数据时的写数据指令头。

DATA_LEFT_SHIFT_COUNT：指明DATA段流程中，数据与指令拼接时的规则，此处是指出数据左移的位数，即指令如下：内核指令 = 指令头（DMA_CMD_HEAD/ REG_WRITE_DATA_HEAD） + 数据<< DATA_LEFT_SHIFT_COUNT。

ADDRESS_LEFT_SHIFT_COUNT：指明DATA段流程中，不使用DMA时地址与指令拼接时的规则，此处是指出地址左移的位数，即指令如下：内核指令 = 指令头（REG_WRITE_ADDRESS_HEAD） + 地址<< DATA_LEFT_SHIFT_COUNT。

组[VAR]包含一些变量信息，所有变量均可通过命令文件中的控制字修改。
JTAG_EMPTY_CLOCK_BETWEEN_CMD：指明两条指令间的空TCK个数。

JTAG_FREQ_MULT：指明TCK的乘数，基础为两条指令为一个TCK周期，配置此项后一个TCK周期为2* JTAG_FREQ_MULT。


命令文件中的控制字：（控制字填在TAP state栏中，值填在TDI栏中（认为是16进制数））
#DELAY：延时多少个TCK指令周期。
#CLOCK：插入多少个TCK时钟。
#SET_EMPTY_CLOCK：设置此条指令之后的，每两条jtag指令之间的空TCK clock个数。
#SET_FREQ_MULT：设置TCK的乘数。