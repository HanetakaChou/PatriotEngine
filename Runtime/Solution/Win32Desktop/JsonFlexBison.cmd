CD /d %~dp0
win_flex -X -o ../../Private/XGL/JSON/JsonLex.inl ../../Private/XGL/JSON/JsonLex.l  
win_bison -o ../../Private/XGL//JSON/JsonYacc.inl -p json_yy ../../Private/XGL/JSON/JsonYacc.y -L C 
REM win_bison -y -o ../../Private/XGL//JSON/JsonParserYacc.inl -p json_yy ../../Private/XGL/JSON/jsonParser.y -L C
