##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=UFFSDB
ConfigurationName      :=Debug
WorkspacePath          := "/home/sr_ogliari/CodeLite/UFFS"
ProjectPath            := "/home/sr_ogliari/CodeLite/UFFS"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Eduardo Ogliari
Date                   :=09/04/16
CodeLitePath           :="/home/sr_ogliari/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="UFFSDB.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lfl  -pthread
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall  -DDEBUG  $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/Fonte_database.c$(ObjectSuffix) $(IntermediateDirectory)/Fonte_uffsdb.c$(ObjectSuffix) $(IntermediateDirectory)/Fonte_dictionary.c$(ObjectSuffix) $(IntermediateDirectory)/Fonte_sqlcommands.c$(ObjectSuffix) $(IntermediateDirectory)/Fonte_misc.c$(ObjectSuffix) $(IntermediateDirectory)/Fonte_buffer.c$(ObjectSuffix) $(IntermediateDirectory)/interface_parser.c$(ObjectSuffix) $(IntermediateDirectory)/interface_y.tab.c$(ObjectSuffix) $(IntermediateDirectory)/interface_lex.yy.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Fonte_database.c$(ObjectSuffix): Fonte/database.c $(IntermediateDirectory)/Fonte_database.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/database.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Fonte_database.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Fonte_database.c$(DependSuffix): Fonte/database.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Fonte_database.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Fonte_database.c$(DependSuffix) -MM "Fonte/database.c"

$(IntermediateDirectory)/Fonte_database.c$(PreprocessSuffix): Fonte/database.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Fonte_database.c$(PreprocessSuffix) "Fonte/database.c"

$(IntermediateDirectory)/Fonte_uffsdb.c$(ObjectSuffix): Fonte/uffsdb.c $(IntermediateDirectory)/Fonte_uffsdb.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/uffsdb.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Fonte_uffsdb.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Fonte_uffsdb.c$(DependSuffix): Fonte/uffsdb.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Fonte_uffsdb.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Fonte_uffsdb.c$(DependSuffix) -MM "Fonte/uffsdb.c"

$(IntermediateDirectory)/Fonte_uffsdb.c$(PreprocessSuffix): Fonte/uffsdb.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Fonte_uffsdb.c$(PreprocessSuffix) "Fonte/uffsdb.c"

$(IntermediateDirectory)/Fonte_dictionary.c$(ObjectSuffix): Fonte/dictionary.c $(IntermediateDirectory)/Fonte_dictionary.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/dictionary.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Fonte_dictionary.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Fonte_dictionary.c$(DependSuffix): Fonte/dictionary.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Fonte_dictionary.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Fonte_dictionary.c$(DependSuffix) -MM "Fonte/dictionary.c"

$(IntermediateDirectory)/Fonte_dictionary.c$(PreprocessSuffix): Fonte/dictionary.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Fonte_dictionary.c$(PreprocessSuffix) "Fonte/dictionary.c"

$(IntermediateDirectory)/Fonte_sqlcommands.c$(ObjectSuffix): Fonte/sqlcommands.c $(IntermediateDirectory)/Fonte_sqlcommands.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/sqlcommands.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Fonte_sqlcommands.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Fonte_sqlcommands.c$(DependSuffix): Fonte/sqlcommands.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Fonte_sqlcommands.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Fonte_sqlcommands.c$(DependSuffix) -MM "Fonte/sqlcommands.c"

$(IntermediateDirectory)/Fonte_sqlcommands.c$(PreprocessSuffix): Fonte/sqlcommands.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Fonte_sqlcommands.c$(PreprocessSuffix) "Fonte/sqlcommands.c"

$(IntermediateDirectory)/Fonte_misc.c$(ObjectSuffix): Fonte/misc.c $(IntermediateDirectory)/Fonte_misc.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/misc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Fonte_misc.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Fonte_misc.c$(DependSuffix): Fonte/misc.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Fonte_misc.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Fonte_misc.c$(DependSuffix) -MM "Fonte/misc.c"

$(IntermediateDirectory)/Fonte_misc.c$(PreprocessSuffix): Fonte/misc.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Fonte_misc.c$(PreprocessSuffix) "Fonte/misc.c"

$(IntermediateDirectory)/Fonte_buffer.c$(ObjectSuffix): Fonte/buffer.c $(IntermediateDirectory)/Fonte_buffer.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/buffer.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Fonte_buffer.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Fonte_buffer.c$(DependSuffix): Fonte/buffer.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Fonte_buffer.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Fonte_buffer.c$(DependSuffix) -MM "Fonte/buffer.c"

$(IntermediateDirectory)/Fonte_buffer.c$(PreprocessSuffix): Fonte/buffer.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Fonte_buffer.c$(PreprocessSuffix) "Fonte/buffer.c"

$(IntermediateDirectory)/interface_parser.c$(ObjectSuffix): Fonte/interface/parser.c $(IntermediateDirectory)/interface_parser.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/interface/parser.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/interface_parser.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/interface_parser.c$(DependSuffix): Fonte/interface/parser.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/interface_parser.c$(ObjectSuffix) -MF$(IntermediateDirectory)/interface_parser.c$(DependSuffix) -MM "Fonte/interface/parser.c"

$(IntermediateDirectory)/interface_parser.c$(PreprocessSuffix): Fonte/interface/parser.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/interface_parser.c$(PreprocessSuffix) "Fonte/interface/parser.c"

$(IntermediateDirectory)/interface_y.tab.c$(ObjectSuffix): Fonte/interface/y.tab.c $(IntermediateDirectory)/interface_y.tab.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/interface/y.tab.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/interface_y.tab.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/interface_y.tab.c$(DependSuffix): Fonte/interface/y.tab.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/interface_y.tab.c$(ObjectSuffix) -MF$(IntermediateDirectory)/interface_y.tab.c$(DependSuffix) -MM "Fonte/interface/y.tab.c"

$(IntermediateDirectory)/interface_y.tab.c$(PreprocessSuffix): Fonte/interface/y.tab.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/interface_y.tab.c$(PreprocessSuffix) "Fonte/interface/y.tab.c"

$(IntermediateDirectory)/interface_lex.yy.c$(ObjectSuffix): Fonte/interface/lex.yy.c $(IntermediateDirectory)/interface_lex.yy.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/sr_ogliari/CodeLite/UFFS/Fonte/interface/lex.yy.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/interface_lex.yy.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/interface_lex.yy.c$(DependSuffix): Fonte/interface/lex.yy.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/interface_lex.yy.c$(ObjectSuffix) -MF$(IntermediateDirectory)/interface_lex.yy.c$(DependSuffix) -MM "Fonte/interface/lex.yy.c"

$(IntermediateDirectory)/interface_lex.yy.c$(PreprocessSuffix): Fonte/interface/lex.yy.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/interface_lex.yy.c$(PreprocessSuffix) "Fonte/interface/lex.yy.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


