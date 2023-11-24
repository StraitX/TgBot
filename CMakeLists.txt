cmake_minimum_required(VERSION 3.10)

add_subdirectory(thirdparty/tgbot-cpp)

add_library(StraitXTgBot
	"./sources/tgbot/simple_bot.cpp"
)

target_link_libraries(StraitXTgBot
	PUBLIC TgBot
	PUBLIC StraitXCore
)

target_include_directories(StraitXTgBot
	PUBLIC "./sources"
)