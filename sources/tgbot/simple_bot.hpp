#pragma once

#include <core/string.hpp>
#include <core/list.hpp>
#include <core/function.hpp>
#include <core/format.hpp>
#include <core/span.hpp>
#include <tgbot/Bot.h>

struct KeyboardButton {
    String Text;
    String CallbackData;

    KeyboardButton() = default;

    KeyboardButton(const KeyboardButton &) = default;

    KeyboardButton(KeyboardButton &&) = default;

    KeyboardButton &operator=(const KeyboardButton &) = default;

    KeyboardButton &operator=(KeyboardButton &&) = default;


    KeyboardButton(String text, String callback_data) :
        Text(Move(text)),
        CallbackData(Move(callback_data))
    {}

    KeyboardButton(String text) :
        Text(text),
        CallbackData(text)
    {}
};

using KeyboardLayout = List<List<KeyboardButton>>;

namespace Keyboard {
    KeyboardLayout ToKeyboard(ConstSpan<String> texts);
}

class SimpleBot: private TgBot::Bot {
    Function<void(String)> m_OnLog;
public:
    SimpleBot(const String& token);

    int Run();

    TgBot::Message::Ptr SendMessage(const TgBot::Message::Ptr& source, const String& message);

    TgBot::Message::Ptr SendMessage(const TgBot::Message::Ptr& source, const String& message, TgBot::GenericReply::Ptr reply);

    TgBot::Message::Ptr SendKeyboard(const TgBot::Message::Ptr& source, const String& message, const KeyboardLayout& keyboard);

    void EditMessage(TgBot::Message::Ptr message, const String &text, TgBot::GenericReply::Ptr reply);

    void EditMessage(TgBot::Message::Ptr message, const String &text, const KeyboardLayout &keyboard);

    void EditMessage(TgBot::Message::Ptr message, const KeyboardLayout &keyboard);

    void EditMessage(TgBot::Message::Ptr message, const String &text);

    void DeleteMessage(TgBot::Message::Ptr message);

    bool AnswerCallbackQuery(const std::string& callbackQueryId, const String& text = String::Empty);
    
    template<typename...ArgsType>
    void Log(const char *fmt, const ArgsType&...args) {
        m_OnLog(Format(fmt, args...));
    }

    void BindOnMessage(Function<void(TgBot::Message::Ptr)> callback);

    void BindOnUnknownCommand(Function<void(TgBot::Message::Ptr)> callback);

    void BindOnCommand(const String &command, Function<void(TgBot::Message::Ptr)> callback);

    void BindOnCommand(std::initializer_list<std::string> commands, Function<void(TgBot::Message::Ptr)> callback);

    void BindOnCallbackQuery(Function<void(TgBot::CallbackQuery::Ptr)> callback);

    void BindOnLog(Function<void(String)> callback);
};