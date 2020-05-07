win_flex -X -o ../../Private/XGL/JsonScannerLex.inl ../../Private/XGL/JsonScanner.l  
win_bison -y -o ../../Private/XGL/JsonParserYacc.inl -p json_yy ../../Private/XGL/jsonParser.y -L C 