CD /d %~dp0
win_flex -X -o ../../Private/XGL/JSON/JsonScannerLex.inl ../../Private/XGL/JSON/JsonScanner.l  
REM win_bison -y -o ../../Private/XGL//JSON/JsonParserYacc.inl -p json_yy ../../Private/XGL/JSON/jsonParser.y -L C
win_bison -o ../../Private/XGL//JSON/JsonParserYacc.inl -p json_yy ../../Private/XGL/JSON/jsonParser.y -L C 