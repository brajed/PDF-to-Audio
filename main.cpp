#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QHttpMultiPart>
#include <QFile>
#include <QTextStream>


void convertPdfToAudio(const QString &filePath, QLabel *statusLabel){

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest request(QUrl("http://127.0.0.1:5000/convert"));
    QFile *file = new QFile(filePath);

    if (!file->open(QIODevice::ReadOnly)){
        statusLabel->setText("Failed to open the File");
        return;
    }

    QHttpMultiPart *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+file->fileName()+"\""));
    filePart.setBodyDevice(file);
    file->setParent(multipart);
    multipart->append(filePart);

    QNetworkReply *reply = manager->post(request, multipart);
    multipart->setParent(reply);

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            statusLabel->setText("Converted Successfully");
        }
        else{
            statusLabel->setText("Conversion Failed" + reply->errorString());
        }
        reply->deleteLater();
        manager->deleteLater();
    });

}

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    QWidget mainWindow;

    mainWindow.setWindowTitle("PDF To Audio App");
    mainWindow.resize(400,300);

    QVBoxLayout *layout = new QVBoxLayout(&mainWindow);

    QLabel *titleLabel = new QLabel("PDF to Audio App");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color:red; font-size:24px");
    layout->addWidget(titleLabel);

    QPushButton *chooseButton = new QPushButton("Choose Pdf File");
    chooseButton->setStyleSheet("color:white; background-color:gray; font-size: 18px");
    layout->addWidget(chooseButton);

    QLabel *filenameLabel = new QLabel("");
    filenameLabel->setAlignment(Qt::AlignCenter);
    filenameLabel->setStyleSheet("color:red; font-size:20px");
    layout->addWidget(filenameLabel);

    QPushButton *convertButton = new QPushButton("Convert");
    convertButton->setStyleSheet("color:white; background-color:green; font-size: 18px");
    convertButton->hide();
    layout->addWidget(convertButton);

    QLabel *statusLabel = new QLabel("Status Label");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color:pink; font-size:20px");
    statusLabel->hide();
    layout->addWidget(statusLabel);

    QObject::connect(chooseButton, &QPushButton::clicked, [&](){
        QString filePath = QFileDialog ::getOpenFileName(&mainWindow, "Choose PDF File", "","PDF Files (*.pdf)");

        if(!filePath.isEmpty()){
            filenameLabel->setText(filePath);
            convertButton->show();
        }
        else{
            statusLabel->setText("Please Choose a PDF File First");
        }
    });

    QObject::connect(convertButton, &QPushButton::clicked, [&](){
        QString filePath = filenameLabel->text();
        statusLabel->show();

        if(!filePath.isEmpty()){
            statusLabel->setText("Converting...");
            convertPdfToAudio(filePath, statusLabel);
        }
    });

    mainWindow.show();

    app.exec();
}
