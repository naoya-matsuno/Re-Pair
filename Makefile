# ヘッダーファイルのディレクトリ
INC_DIR := ./include

# コンパイラ設定
CXX := g++
CXXFLAGS := -std=c++20 -Wall -I $(INC_DIR)

# ソースファイルのディレクトリ
SRC_DIR := ./src

# 実験用のソースファイルのディレクトリ
EXPERIMENT_DIR := ./experiment

# オブジェクトファイルのディレクトリ
OBJ_DIR := ./obj

# 実行ファイルのディレクトリ
BIN_DIR := ./bin

# テスト用のソースファイルのディレクトリ
TEST_DIR := ./test

# ヘッダーのファイル名
HEADER_FILE_NAMES := types repair external_data_handler

# ソースファイル
SRCS := $(addprefix $(SRC_DIR)/, $(patsubst %, %.cpp, $(HEADER_FILE_NAMES)))

# オブジェクトファイル
OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %, %.o, $(HEADER_FILE_NAMES)))

# ヘッダーファイル
INCS := $(wildcard $(INC_DIR)/*.hpp)

# オブジェクトファイルの生成
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCS)
	@if [ ! -d ./obj ]; then\
		mkdir ./obj;\
	fi
	$(CXX) $(CXXFLAGS) -c -o $@ $<  

# 実行ファイルの生成
$(BIN_DIR)/%test: $(TEST_DIR)/%test.cpp $(OBJS)
	@if [ ! -d ./bin ]; then\
		mkdir ./bin;\
	fi
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BIN_DIR)/%: $(EXPERIMENT_DIR)/%.cpp $(OBJS)
	@if [ ! -d ./bin ]; then\
		mkdir ./bin;\
	fi
	$(CXX) $(CXXFLAGS) -o $@ $^

# 実行コマンド
main: $(BIN_DIR)/main
	$(BIN_DIR)/main

types_test: $(BIN_DIR)/types_test
	$(BIN_DIR)/types_test

clean:
	rm ./obj/* ./bin/*