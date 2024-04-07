# Re-Pair
Re-Pair compression algorithm.

## includeフォルダ
ここにヘッダーファイルを置く．

## srcフォルダ
ここにヘッダーの実装を書いたソースファイルを置く．（何も書かない場合でも置く）

## experimentフォルダ
実験用のコードを置く．名前は"~_experiment.cpp"とする．

## testフォルダ
テストコードを置く．名前は"~_test.cpp"とする．

## Makefile
- ヘッダーファイルの名前を"HEADER_FILE_NAMES"に書く．

- 実行コマンドとして以下を記述する．

```dif_makefile
（実験，テストコードの書かれたソースファイルの名前）: $(BIN_DIR)/（実験，テストコードの書かれたソースファイルの名前）
    $(BIN_DIR)/（実験，テストコードの書かれたソースファイルの名前）
```

- 実行は"make （コマンド名）"．
