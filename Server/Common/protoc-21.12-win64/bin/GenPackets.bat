pushd %~dp0

protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=S_ --send=C_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=C_ --send=S_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"
XCOPY /Y ServerPacketHandler.h "../../../GameServer"

XCOPY /Y Enum.pb.h "../../../DummyClient"
XCOPY /Y Enum.pb.cc "../../../DummyClient"
XCOPY /Y Struct.pb.h "../../../DummyClient"
XCOPY /Y Struct.pb.cc "../../../DummyClient"
XCOPY /Y Protocol.pb.h "../../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../../DummyClient"
XCOPY /Y ClientPacketHandler.h "../../../DummyClient"

XCOPY /Y Enum.pb.h "../../../../R3/Source/R3/Network"
XCOPY /Y Enum.pb.cc "../../../../R3/Source/R3/Network"
XCOPY /Y Struct.pb.h "../../../../R3/Source/R3/Network"
XCOPY /Y Struct.pb.cc "../../../../R3/Source/R3/Network"
XCOPY /Y Protocol.pb.h "../../../../R3/Source/R3/Network"
XCOPY /Y Protocol.pb.cc "../../../../R3/Source/R3/Network"
XCOPY /Y ClientPacketHandler.h "../../../../R3/Source/R3"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE