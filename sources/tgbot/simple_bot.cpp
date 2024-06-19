#include "simple_bot.hpp"
#include <core/move.hpp>
#include <tgbot/net/TgLongPoll.h>

KeyboardLayout Keyboard::ToKeyboard(ConstSpan<String> texts) {
    KeyboardLayout layout;
    List<KeyboardButton> row;

    for (const auto &text : texts) {
        row.Emplace(KeyboardButton(text));
    }

    layout.Emplace(std::move(row));

    return layout;
}

TgBot::InlineKeyboardMarkup::Ptr ToInlineKeyboardMarkup(const KeyboardLayout &keyboard) {

    TgBot::InlineKeyboardMarkup::Ptr keyboard_markup(new TgBot::InlineKeyboardMarkup);

    for (const auto& row : keyboard) {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row_markup;

        for (const auto& button : row) {
            TgBot::InlineKeyboardButton::Ptr button_markup(new TgBot::InlineKeyboardButton);
            button_markup->text = button.Text;
            button_markup->callbackData = button.CallbackData;

            row_markup.push_back(button_markup);
        }

        keyboard_markup->inlineKeyboard.push_back(row_markup);
    }
    
    return keyboard_markup;
}

SimpleBot::SimpleBot(const String& token, const String &parse_mode):
    TgBot::Bot(token),
    m_ParseMode(parse_mode)
{}

int SimpleBot::Run() {
    Log("Started");

    try {
        TgBot::TgLongPoll long_poll(*this);

        while (true) {
            long_poll.start();
        }

    }
    catch (const std::exception& exception) {
        Log("Crash: %", exception.what());
    }

    return EXIT_SUCCESS;
}

TgBot::Message::Ptr SimpleBot::SendMessage(const TgBot::Message::Ptr& source, const String& message) {
    return SendMessage(source, message, nullptr);
}

TgBot::Message::Ptr SimpleBot::SendMessage(const TgBot::Message::Ptr& source, const String& message, TgBot::GenericReply::Ptr reply) {
    if (!message.Size()) {
        Log("Can't send empty messages");
        return nullptr;
    }

    TgBot::Message::Ptr result = nullptr;

    try {
        result = getApi().sendMessage(source->chat->id, message, m_DisableWebpagePreview, 0, reply, m_ParseMode, false, {}, false, false, source->isTopicMessage ? source->messageThreadId : 0);
    }
    catch (const std::exception& exception) {
        auto chat = source->chat;

        auto chat_name = chat->username.size() ? chat->username : chat->title;

        Log("Failed to send message to chat '%' id % reason %", chat_name, chat->id, exception.what());
    }

    return result;
}

void SimpleBot::EditMessage(TgBot::Message::Ptr message, const String& text, const KeyboardLayout& keyboard) {
    EditMessage(message, text, ToInlineKeyboardMarkup(keyboard));
}

TgBot::Message::Ptr SimpleBot::SendKeyboard(const TgBot::Message::Ptr& source, const String& message, const KeyboardLayout& keyboard) {
    return SendMessage(source, message, ToInlineKeyboardMarkup(keyboard));
}

void SimpleBot::EditMessage(TgBot::Message::Ptr message, const String& text, TgBot::GenericReply::Ptr reply) {
    try{
        if (text.size()) {
            getApi().editMessageText(text, message->chat->id, message->messageId, "", m_ParseMode, m_DisableWebpagePreview, reply);
        } else {
            getApi().editMessageReplyMarkup(message->chat->id, message->messageId, "", reply);
        }
    }
    catch (const std::exception& exception) {
        Log("Failed to edit message: %", exception.what());
    }
}

void SimpleBot::EditMessage(TgBot::Message::Ptr message, const KeyboardLayout& keyboard) {
    EditMessage(message, String::Empty, ToInlineKeyboardMarkup(keyboard));
}

void SimpleBot::EditMessage(TgBot::Message::Ptr message, const String& text) {
    EditMessage(message, text, nullptr);
}

bool SimpleBot::AnswerCallbackQuery(const std::string& callbackQueryId, const String& text) {
    
    try{
        return getApi().answerCallbackQuery(callbackQueryId, text);
    }
    catch (const std::exception& exception) {
        Log("Failed to answer callback query %", callbackQueryId);
    }
    return false;
}

void SimpleBot::DeleteMessage(TgBot::Message::Ptr message) {
    try{
        getApi().deleteMessage(message->chat->id, message->messageId);
    }
    catch (const std::exception& exception) {
        auto chat = message->chat;

        auto chat_name = chat->username.size() ? chat->username : chat->title;

        Log("Failed to delete message % from chat %, id %, reason: %", message->messageId, chat_name, chat->id, exception.what());
    }
}

void SimpleBot::BindOnMessage(Function<void(TgBot::Message::Ptr)> callback){
    getEvents().onNonCommandMessage([callback = std::move(callback)](auto message) mutable { callback.TryCall(message); });
}

void SimpleBot::BindOnUnknownCommand(Function<void(TgBot::Message::Ptr)> callback) {
    getEvents().onUnknownCommand([callback = std::move(callback)](auto message) mutable { callback.TryCall(message); });
}

void SimpleBot::BindOnCommand(const String &command, Function<void(TgBot::Message::Ptr)> callback) {
    getEvents().onCommand(command, [callback = std::move(callback)](auto message) mutable { callback.TryCall(message); });
}

void SimpleBot::BindOnCommand(std::initializer_list<std::string> commands, Function<void(TgBot::Message::Ptr)> callback) {
    getEvents().onCommand(commands, [callback = std::move(callback)](auto message) mutable { callback.TryCall(message); });
}

void SimpleBot::BindOnCallbackQuery(Function<void(TgBot::CallbackQuery::Ptr)> callback) {
    getEvents().onCallbackQuery([callback = std::move(callback)](auto query) mutable { callback.TryCall(query); });
}

void SimpleBot::BindOnLog(Function<void(String)> callback) {
    m_OnLog = callback;
}
