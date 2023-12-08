#include "webrequestwidget.h"

WebContext *WebRequestWidget::getWebContext() const
{
    return webContext;
}

void WebRequestWidget::setWebContext(WebContext *newWebContext)
{
    webContext = newWebContext;
}

WebRequestWidget::WebRequestWidget(QWidget *parent)
    : QWidget{parent}
{
    // инициализация полей и их связей
    requestBody = new WebRequestBody(this);
    connect(this, SIGNAL(jsonObjectReady(QJsonObject)),
            requestBody, SLOT(setJsonData(QJsonObject)));
    dataTextEdit = new QTextEdit(this);
    urlLineEdit = new QLineEdit(this);
    parseStatusLabel = new QLabel("Status");
    sendRequestButton = new QPushButton("Отправить", this);
    connect(sendRequestButton, SIGNAL(pressed()), this, SLOT(sendJson()));

    // инициализация других виджетов и их связей
    QLabel *lineEditLabel = new QLabel("&Адрес запроса:", this);
    lineEditLabel->setBuddy(dataTextEdit);
    QLabel *textEditLabel = new QLabel("&Тело запроса:", this);
    textEditLabel->setBuddy(dataTextEdit);
    QPushButton *parseButton = new QPushButton("Проверить тело запроса", this);
    connect(parseButton, SIGNAL(pressed()), this, SLOT(tryParseJson()));

    // установки компоновщика
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(lineEditLabel);
    layout->addWidget(urlLineEdit);
    layout->addWidget(textEditLabel);
    layout->addWidget(dataTextEdit);
    layout->addWidget(parseStatusLabel);
    layout->addWidget(parseButton);
    layout->addWidget(sendRequestButton);
}

void WebRequestWidget::tryParseJson()
{
    // пытаемся запарсить
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::
        fromJson(dataTextEdit->toPlainText().toUtf8(), &error);
    // выводим успешность парсинга
    parseStatusLabel->setText(error.errorString());
}

void WebRequestWidget::sendJson()
{
    // парсим
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::
        fromJson(dataTextEdit->toPlainText().toUtf8(), &error);
    // если все в порядке
    if (error.error == error.NoError) {
        emit jsonObjectReady(doc.object());
    }
    // анализируем текущий обьект джейсона, если впорядке - отправляем
    QJsonObject curJson = requestBody->getJsonData();
    if(!curJson.isEmpty() && urlLineEdit->text().length() > 0) {
        webContext->sendPostRequest(urlLineEdit->text(), curJson);
        parseStatusLabel->setText("succesful sended");
    } else {
        parseStatusLabel->setText("error while sending");
    }
}
