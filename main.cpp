#include <QtWidgets>
#include <QtWebEngineWidgets>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        QUrl url("https://www.virustotal.com/vtapi/v2/file/scan");
        url.addQueryItem("apikey", "YOUR_API_KEY"); // Замените YOUR_API_KEY на ваш API-ключ VirusTotal

        webView = new QWebEngineView(this);
        webView->setUrl(QUrl("about:blank"));

        QVBoxLayout *layout = new QVBoxLayout();
        QPushButton *chooseFileButton = new QPushButton("Выбрать файл");
        layout->addWidget(chooseFileButton);
        layout->addWidget(webView);

        QWidget *centralWidget = new QWidget(this);
        centralWidget->setLayout(layout);
        setCentralWidget(centralWidget);

        connect(chooseFileButton, &QPushButton::clicked, this, &MainWindow::chooseFile);

        manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::onReplyFinished);
    }

private slots:
    void chooseFile()
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл для проверки");
        if (!filePath.isEmpty())
        {
            QFileInfo fileInfo(filePath);
            currentFile = fileInfo;
            uploadFile(filePath);
        }
    }

    void uploadFile(const QString &filePath)
    {
        QNetworkRequest request(QUrl("https://www.virustotal.com/vtapi/v2/file/scan"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");

        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"" + currentFile.fileName() + "\""));
        QFile *file = new QFile(filePath);
        file->open(QIODevice::ReadOnly);
        filePart.setBodyDevice(file);
        file->setParent(multiPart);
        multiPart->append(filePart);

        QNetworkReply *reply = manager->post(request, multiPart);
        multiPart->setParent(reply);
    }

    void onReplyFinished(QNetworkReply *reply)
    {
        QByteArray data = reply->readAll();
        QString response = QString::fromUtf8(data);

        webView->setHtml(response);

        reply->deleteLater();
    }

private:
    QWebEngineView *webView;
    QFileInfo currentFile;
    QNetworkAccessManager *manager;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

#include "main.moc"
