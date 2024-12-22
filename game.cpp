#include "game.h"
#include "button.h"
#include <QPixmap>
#include "king.h"
#include <QDebug>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include "clientmanager.h"
#include "user.h"
#include "player.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QScrollBar>
#include <QTimer>


QString Game::role = ""; // Khởi tạo giá trị mặc định
QString Game::match_id = "";
QList<QGraphicsItem*> listPlayerItems;

Game::Game(QWidget *parent ):QGraphicsView(parent)
{
    // Tạo Scene cho game
    gameScene = new QGraphicsScene();
    gameScene->setSceneRect(0,0,1400,900);

    // Tạo view
    setFixedSize(1400,900);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Tắt thanh cuộn ngang
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Tắt thanh cuộn dọc
    setScene(gameScene);

    // Đặt màu nền là đen
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::black);
    setBackgroundBrush(brush);
    pieceToMove = NULL;

    // Hiển thị lượt đi hiện tại
    turnDisplay = new QGraphicsTextItem();
    turnDisplay->setPos(width()/2-100,10); // Vị trí của text hiển thị lượt
    turnDisplay->setZValue(1);
    turnDisplay->setDefaultTextColor(Qt::white);
    turnDisplay->setFont(QFont("",18));
    turnDisplay->setPlainText("Turn : WHITE");

    // Hiển thị thông báo "CHECK" (CHECK trong cờ vua là chiếu tướng)
    check = new QGraphicsTextItem();
    check->setPos(width()/2-100,860); // Vị trí của thông báo "CHECK"
    check->setZValue(4);
    check->setDefaultTextColor(Qt::red);
    check->setFont(QFont("",18));
    check->setPlainText("CHECK!");
    check->setVisible(false); // Ẩn thông báo ban đầu
    setTurn("WHITE"); // Thiết lập lượt đi ban đầu là "WHITE"

}

void Game::setBackground() {
    // Kiểm tra và xóa nền cũ nếu có
    if (background) {
        gameScene->removeItem(background);
        delete background;
        background = nullptr;
    }

    // Thêm nền mới
    background = new QGraphicsPixmapItem(QPixmap(":/images/background.jpg"));
    background->setZValue(-1); // Đặt Z-value thấp nhất để luôn ở phía sau

    background->setPixmap(QPixmap(":/images/background.jpg").scaled(1400, 900));

    background->setPos(0, 0); // Đặt vị trí của background

    background->setOpacity(0.5);

    gameScene->addItem(background); // Thêm nền vào Scene
}

void Game::drawChessBoard(QString role)
{
    chess = new ChessBoard(role);
    drawDeadHolder(0,0,Qt::lightGray);    // Khu vực chứa quân cờ trắng đã chết
    drawDeadHolder(1100,0,Qt::lightGray); // Khu vực chứa quân cờ đen đã chết
    chess->drawBoxes(width()/2-400,50);   // Vẽ bàn cờ ở vị trí trung tâm
}

void Game::displayDeadWhite()
{
    int SHIFT = 50;
    int j = 0;
    int k = 0;
    for(size_t i = 0,n = whiteDead.size(); i<n; i++) {
        if(j == 4){
            k++;
            j = 0;
        }
        whiteDead[i]->setPos(40+SHIFT*j++,100+SHIFT*2*k); // Đặt vị trí của quân cờ trắng đã chết
    }
}

void Game::displayDeadBlack()
{
    int SHIFT = 50;
    int j = 0;
    int k = 0;
    for(size_t i = 0,n = blackDead.size(); i<n; i++) {
        if(j == 4){
            k++;
            j = 0;
        }
        blackDead[i]->setPos(1140+SHIFT*j++,100+SHIFT*2*k); // Đặt vị trí của quân cờ đen đã chết
    }
}

void Game::placeInDeadPlace(ChessPiece *piece)
{
    // Kiểm tra quân cờ đã chết thuộc màu nào
    if(piece->getSide() == "WHITE") {
        whiteDead.append(piece);
        King *g = dynamic_cast <King *>(piece);
        if(g){
            // Nếu quân vua trắng bị bắt, đen thắng
            check->setPlainText("Black Won");
            gameOver();
        }
        displayDeadWhite();
    }
    else{
        blackDead.append(piece);
        King *g = dynamic_cast <King *>(piece);
        if(g){
            // Nếu quân vua đen bị bắt, trắng thắng
            check->setPlainText("White Won");
            gameOver();
        }
        displayDeadBlack();
    }
    alivePiece.removeAll(piece); // Loại bỏ quân cờ đã chết khỏi danh sách quân cờ sống
}

void Game::addToScene(QGraphicsItem *item)
{
    gameScene->addItem(item); // Thêm đối tượng vào Scene
}

void Game::removeFromScene(QGraphicsItem *item)
{
    gameScene->removeItem(item); // Xóa đối tượng khỏi Scene
}

QString Game::getTurn()
{
    return turn; // Lấy lượt đi hiện tại
}

void Game::setTurn(QString value)
{
    turn = value; // Thiết lập lượt đi
}

void Game::changeTurn()
{
    if(getTurn() == "WHITE")
        setTurn("BLACK");
    else
        setTurn("WHITE");
    turnDisplay->setPlainText("Turn : " + getTurn()); // Cập nhật hiển thị lượt
}

void Game::start(const QString &status, const QString &match_id, const QString &opponent, const QString &role)
{
    if (background) {
        gameScene->removeItem(background);
        delete background;
        background = nullptr;
    }
    // drawChessBoard("WHITE");
    drawChessBoard(role);

    // Xóa các đối tượng khỏi Scene
    clearScene();
    Game::role = role; // Gán giá trị mới cho biến static
    Game::match_id = match_id;
    qDebug()<< " match_id :" << Game::match_id;
    addToScene(turnDisplay);
    QGraphicsTextItem* whitePiece = new QGraphicsTextItem();
    whitePiece->setPos(70,10);
    whitePiece->setZValue(1);
    whitePiece->setDefaultTextColor(Qt::white);
    whitePiece->setFont(QFont("",14));
    whitePiece->setPlainText("WHITE PIECE");
    addToScene(whitePiece);
    QGraphicsTextItem *blackPiece = new QGraphicsTextItem();

    blackPiece->setPos(1170,10);
    blackPiece->setZValue(1);
    blackPiece->setDefaultTextColor(Qt::black);
    blackPiece->setFont(QFont("",14));
    blackPiece->setPlainText("BLACK PIECE");
    addToScene(blackPiece);
    addToScene(check); // Thêm thông báo "CHECK"
    chess->addChessPiece(role); // Thêm các quân cờ vào bàn cờ

}

void Game::drawDeadHolder(int x, int y,QColor color)
{
    // Tạo vùng chứa quân cờ đã chết
    deadHolder = new QGraphicsRectItem(x,y,300,900);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    deadHolder->setBrush(brush);
    addToScene(deadHolder); // Thêm vùng chứa vào Scene
}


QList<ChessPiece*> Game::getAllChessPieces() {
    return this->alivePiece; // Danh sách chứa tất cả các quân cờ
}


// Hàm hiển thị màn hình đăng nhập
extern ClientManager *clientManager;
extern User *user;
extern QList<Player*> players;
void Game::displayLogin() {
    setBackground();

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Chess Pro");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::white);
    int xPos = width()/2 - titleText->boundingRect().width()/2;
    int yPos = 150;
    titleText->setPos(xPos, yPos);
    addToScene(titleText);
    listG.append(titleText);

    // Tạo trường nhập tên người dùng
    QLineEdit *usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter username");
    usernameInput->setFixedSize(300, 50);
    QFont usernameFont("Arial", 16);
    usernameInput->setFont(usernameFont);
    QGraphicsProxyWidget *proxyUsername = gameScene->addWidget(usernameInput);
    proxyUsername->setPos(width()/2 - usernameInput->width()/2, 270);
    listG.append(proxyUsername);

    // Tạo trường nhập mật khẩu
    QLineEdit *passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setFixedSize(300, 50);
    QFont passwordFont("Arial", 16);
    passwordInput->setFont(passwordFont);
    QGraphicsProxyWidget *proxyPassword = gameScene->addWidget(passwordInput);
    proxyPassword->setPos(width()/2 - passwordInput->width()/2, 340);
    listG.append(proxyPassword);

    QFont logFont("Arial", 14);

    QGraphicsTextItem *errorText = new QGraphicsTextItem("");
    errorText->setDefaultTextColor(Qt::red);
    errorText->setFont(logFont);
    addToScene(errorText);
    listG.append(errorText);

    // Nút đăng nhập
    Button *loginButton = new Button("Login");
    int btnXPos = width()/2 - loginButton->boundingRect().width()/2;
    int btnYPos = 450;
    loginButton->setPos(btnXPos, btnYPos);
    connect(loginButton, &Button::clicked, this, [=]() {
        QString username = usernameInput->text();
        QString password = passwordInput->text();

        // Kiểm tra nếu có trường nào còn trống
        if (username.isEmpty() || password.isEmpty()) {
            qDebug() << "All fields must be filled!";
            errorText->setPlainText("All fields must be filled!");
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            return;
        }

        if (clientManager) {
            clientManager->sendLoginRequest(username, password);
        }

        connect(clientManager, &ClientManager::loginResult, this, [=](const QString &status, const QString &message, const QString &name, int elo, const QString &token) {
            if (status == "success") {
                // Giải mã token để lấy username
                user->setUsername(username);
                user->setPassword(password);
                user->setName(name);
                user->setToken(token);
                user->setElo(elo);
                displayWaitConnect();

            } else if (status == "failure") {
                errorText->setPlainText(message);
                errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 700);
            }
        });
    });
    addToScene(loginButton);
    listG.append(loginButton);

    // Nút đăng ký
    Button *registerButton = new Button("Register");
    registerButton->setPos(btnXPos, btnYPos + 70);
    connect(registerButton, &Button::clicked, this, &Game::displayRegister);
    addToScene(registerButton);
    listG.append(registerButton);

      //drawChessBoard("WHITE");
}

void Game::displayRegister() {
    setBackground();

    clearScene();

    // Tiêu đề
    QGraphicsTextItem *registerTitle = new QGraphicsTextItem("Register");
    QFont registerFont("arial", 50);
    registerTitle->setFont(registerFont);
    registerTitle->setDefaultTextColor(Qt::white);
    registerTitle->setPos(width()/2 - registerTitle->boundingRect().width()/2, 150);
    addToScene(registerTitle);
    listG.append(registerTitle);

    QFont fieldFont("Arial", 16);

    // Trường nhập tên
    QLineEdit *nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Enter your name");
    nameInput->setFixedSize(300, 50);
    nameInput->setFont(fieldFont);
    QGraphicsProxyWidget *proxyName = gameScene->addWidget(nameInput);
    proxyName->setPos(width()/2 - nameInput->width()/2, 270);
    listG.append(proxyName);

    // Trường nhập tài khoản
    QLineEdit *usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter username");
    usernameInput->setFixedSize(300, 50);
    usernameInput->setFont(fieldFont);
    QGraphicsProxyWidget *proxyUsername = gameScene->addWidget(usernameInput);
    proxyUsername->setPos(width()/2 - usernameInput->width()/2, 340);
    listG.append(proxyUsername);

    // Trường nhập mật khẩu
    QLineEdit *passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter password");
    passwordInput->setEchoMode(QLineEdit::Password); // Ẩn mật khẩu
    passwordInput->setFixedSize(300, 50);
    passwordInput->setFont(fieldFont);
    QGraphicsProxyWidget *proxyPassword = gameScene->addWidget(passwordInput);
    proxyPassword->setPos(width()/2 - passwordInput->width()/2, 410);
    listG.append(proxyPassword);

    // Trường xác nhận mật khẩu
    QLineEdit *confirmPasswordInput = new QLineEdit();
    confirmPasswordInput->setPlaceholderText("Confirm password");
    confirmPasswordInput->setEchoMode(QLineEdit::Password); // Ẩn mật khẩu
    confirmPasswordInput->setFixedSize(300, 50);
    confirmPasswordInput->setFont(fieldFont);
    QGraphicsProxyWidget *proxyConfirmPassword = gameScene->addWidget(confirmPasswordInput);
    proxyConfirmPassword->setPos(width()/2 - confirmPasswordInput->width()/2, 480);
    listG.append(proxyConfirmPassword);

    QFont logFont("Arial", 14);

    QGraphicsTextItem *logText = new QGraphicsTextItem("");
    logText->setFont(logFont);
    addToScene(logText);
    listG.append(logText);

    // Nút đăng ký
    Button *registerButton = new Button("Register");
    registerButton->setPos(width()/2 - registerButton->boundingRect().width()/2, 590);
    addToScene(registerButton);
    listG.append(registerButton);

    // Quay lại màn hình đăng nhập
    Button *loginButton = new Button("Back to Login");
    loginButton->setPos(width()/2 - loginButton->boundingRect().width()/2, 660);
    addToScene(loginButton);
    listG.append(loginButton);

    connect(registerButton, &Button::clicked, this, [=]() {
        QString name = nameInput->text();
        QString username = usernameInput->text();
        QString password = passwordInput->text();
        QString confirmPassword = confirmPasswordInput->text();

        // Kiểm tra nếu có trường nào còn trống
        if (name.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
            logText->setPlainText("All fields must be filled!");
            logText->setDefaultTextColor(Qt::red);
            logText->setPos(width()/2 - logText->boundingRect().width()/2, 750);
            return; // Dừng lại không thực hiện đăng ký
        }

        // Kiểm tra mật khẩu xác nhận
        if (password != confirmPassword) {
            logText->setPlainText("Passwords do not match!");
            logText->setDefaultTextColor(Qt::red);
            logText->setPos(width()/2 - logText->boundingRect().width()/2, 750);
            return; // Dừng lại không thực hiện đăng ký
        }

        // Gọi phương thức gửi yêu cầu đăng ký đến server
        if (clientManager) {
            clientManager->sendRegisterRequest(name, username, password);
        }

        connect(clientManager, &ClientManager::registerResult, this, [=](const QString &status, const QString &message) {
            qDebug() << message;
            if (status == "success") {
                logText->setPlainText(message);
                logText->setDefaultTextColor(Qt::green);
                logText->setPos(width()/2 - logText->boundingRect().width()/2, 700);
            } else if (status == "failure") {
                logText->setPlainText(message);
                logText->setDefaultTextColor(Qt::red);
                logText->setPos(width()/2 - logText->boundingRect().width()/2, 700);
            }
        });
    });

    connect(loginButton, &Button::clicked, this, [=]() {
        displayLogin();
    });
}

extern ClientManager *clientManager;
extern User *user;
extern QList<Player*> players;
void Game::displayWaitConnect() {
    setBackground();

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Waitting Connect...");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::white);
    titleText->setPos(width()/2 - titleText->boundingRect().width()/2, 150);
    addToScene(titleText);
    listG.append(titleText);

    QFont logFont("Arial", 36);

    QGraphicsTextItem *logText = new QGraphicsTextItem("");
    logText->setFont(logFont);
    logText->setDefaultTextColor(Qt::white);
    addToScene(logText);
    listG.append(logText);

    if (clientManager) {
        clientManager->sendConnectRequest(user->getToken(), user->getUsername());
    }

    QTimer *timer = new QTimer(this);
    int *counter = new int(0);

    connect(timer, &QTimer::timeout, this, [=]() mutable {
        if (*counter < 10) {
            *counter += 1;
            logText->setPlainText(QString("Waiting... %1").arg(*counter));
            logText->setPos(width()/2 - logText->boundingRect().width()/2, 350);
        } else {
            timer->stop();
        }
    });

    timer->start(1000);

    // Kết nối tín hiệu từ ClientManager
    connect(clientManager, &ClientManager::connectionResult, this, [=](const QString &status, const QString &message) {
        timer->stop();

        if (status == "success") {
            displayMenu("");
        } else if (status == "failure") {
            logText->setPlainText(message);
            logText->setDefaultTextColor(Qt::red);
            logText->setPos(width()/2 - logText->boundingRect().width()/2, 350);

            Button *loginButton = new Button("Back to Login");
            loginButton->setPos(width()/2 - loginButton->boundingRect().width()/2, 500);
            addToScene(loginButton);
            listG.append(loginButton);

            connect(loginButton, &Button::clicked, this, [=]() {
                displayLogin();
            });
        }
    });
}

void Game::displayMenu(const QString &logTextResult) {
    setBackground();

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Chess Pro");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::white);
    titleText->setPos(width()/2 - titleText->boundingRect().width()/2, 150);
    addToScene(titleText);
    listG.append(titleText);

    // Nút tìm trận
    Button *findMatchButton = new Button("Find Match");
    int btnXPos = width()/2 - findMatchButton->boundingRect().width()/2;
    int btnYPos = 300;
    findMatchButton->setPos(width()/2 - findMatchButton->boundingRect().width()/2, 300);
    addToScene(findMatchButton);
    listG.append(findMatchButton);

    // Nút tạo phòng
    Button *createRoomButton = new Button("Create Room");
    createRoomButton->setPos(btnXPos, btnYPos + 100);
    addToScene(createRoomButton);
    listG.append(createRoomButton);

    // Nút quản lý người dùng
    Button *openProfileButton = new Button("Open Profile");
    openProfileButton->setPos(btnXPos, btnYPos + 200);
    addToScene(openProfileButton);
    listG.append(openProfileButton);

    QFont logFont("Arial", 36);

    QGraphicsTextItem *logText = new QGraphicsTextItem(logTextResult);
    logText->setFont(logFont);
    logText->setDefaultTextColor(Qt::red);
    logText->setPos(width()/2 - logText->boundingRect().width()/2, btnYPos + 300);
    addToScene(logText);
    listG.append(logText);

    QFont inviteFont("Arial", 12);
    QGraphicsScene *listInviteScene = new QGraphicsScene();

    connect(findMatchButton, &Button::clicked, this, [=]() {
        displayWaitFindMatch();
    });

    connect(createRoomButton, &Button::clicked, this, [=]() {
        if (clientManager) {
            clientManager->sendCreateRoomRequest(user->getUsername());
        }
    });

    connect(openProfileButton, &Button::clicked, this, [=]() {
        displayProfile();
    });

    connect(clientManager, &ClientManager::createRoomResult, this, [=](const QString &status, const QString &message) {
        if (status == "success") {
            Player emptyPlayer;
            displayRoom(emptyPlayer);
        } else if (status == "failure") {
            logText->setPlainText(message);
            logText->setPos(width()/2 - logText->boundingRect().width()/2, btnYPos + 300);
        }
    });

    connect(clientManager, &ClientManager::invitePlayerResult, this, [=](const QString &username, const QString &name) {
        QGraphicsTextItem *inviteText1 = new QGraphicsTextItem("Invitation from");
        inviteText1->setFont(inviteFont);
        inviteText1->setDefaultTextColor(Qt::black);
        inviteText1->setPos(- 200, 65);
        listInviteScene->addItem(inviteText1);
        QGraphicsTextItem *inviteText2 = new QGraphicsTextItem(name);
        inviteText2->setFont(inviteFont);
        inviteText2->setDefaultTextColor(Qt::black);
        inviteText2->setPos(- 200, 85);
        listInviteScene->addItem(inviteText2);

        Button *acceptButton = new Button("Accept");
        acceptButton->setFont();
        acceptButton->setRect(- 70,50,100,30);
        acceptButton->alignText(- 70,50);
        listInviteScene->addItem(acceptButton);

        Button *declineButton = new Button("Decline");
        declineButton->setFont();
        declineButton->setRect(- 70,90,100,30);
        declineButton->alignText(- 70,90);
        listInviteScene->addItem(declineButton);

        QGraphicsView *listInviteView = new QGraphicsView();
        listInviteView->setScene(listInviteScene);
        listInviteView->setFixedSize(250, 100);

        QGraphicsProxyWidget *proxyInviteWidget = new QGraphicsProxyWidget();
        proxyInviteWidget->setWidget(listInviteView);

        addToScene(proxyInviteWidget);
        listG.append(proxyInviteWidget);

        // Tạo bộ đếm thời gian để ẩn đi sau 10s nếu không có sự tương tác
        QTimer::singleShot(10000, this, [=]() {
            clientManager->sendRespondInviteRequest("decline",user->getUsername(),username);
            removeFromScene(proxyInviteWidget);
            listG.removeAll(proxyInviteWidget);
        });

        connect(acceptButton, &Button::clicked, this, [=]() {
            clientManager->sendRespondInviteRequest("accept",user->getUsername(),username);
            removeFromScene(proxyInviteWidget);
            listG.removeAll(proxyInviteWidget);
        });

        connect(declineButton, &Button::clicked, this, [=]() {
            clientManager->sendRespondInviteRequest("decline",user->getUsername(),username);
            removeFromScene(proxyInviteWidget);
            listG.removeAll(proxyInviteWidget);
        });
    });

    connect(clientManager, &ClientManager::createRoomResult, this, [=](const QString &status, const QString &message) {
        if (status == "success") {
            Player emptyPlayer;
            displayRoom(emptyPlayer);
        } else if (status == "failure") {
            logText->setPlainText(message);
            logText->setPos(width()/2 - logText->boundingRect().width()/2, btnYPos + 300);
        }
    });

    connect(clientManager, &ClientManager::respondInviteResult, this, [=](const QString &status, const QString &message, Player &player) {
        if (status == "success") {
            displayRoom(player);
        } else if (status == "failure") {
            logText->setPlainText(message);
            logText->setPos(width()/2 - logText->boundingRect().width()/2, btnYPos + 300);
        }
    });
}

void Game::displayWaitFindMatch() {
    setBackground();

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Waitting Find Match...");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::white);
    titleText->setPos(width()/2 - titleText->boundingRect().width()/2, 150);
    addToScene(titleText);
    listG.append(titleText);

    QFont logFont("Arial", 36);

    QGraphicsTextItem *logText = new QGraphicsTextItem("");
    logText->setFont(logFont);
    logText->setDefaultTextColor(Qt::white);
    addToScene(logText);
    listG.append(logText);

    if (clientManager) {
        clientManager->sendFindMatchRequest(user->getUsername());
    }

    QTimer *timer = new QTimer(this);
    int *counter = new int(0);

    connect(timer, &QTimer::timeout, this, [=]() mutable {
        if (*counter < 300) {
            *counter += 1;
            logText->setPlainText(QString("Waiting Find Match... %1").arg(*counter));
            logText->setPos(width()/2 - logText->boundingRect().width()/2, 350);
        } else {
            timer->stop();
            displayMenu("Time limited");
        }
    });

    timer->start(1000);

    // Kết nối tín hiệu từ ClientManager
    connect(clientManager, &ClientManager::findMatchResult, this, [=](const QString &status, const QString &message, const QString &opponent, const QString &match_id, const QString &role) {
        timer->stop();
        if (status == "success") {
            start(status,match_id,opponent,role);
        } else if (status == "failure") {
            displayMenu(message);
        }
    });
}

void Game::displayRoom(Player &player) {
    setBackground();

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("CHESS PRO");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::white);
    int xPos = width()/2 - titleText->boundingRect().width()/2;
    int yPos = 150;
    titleText->setPos(xPos, yPos);
    addToScene(titleText);
    listG.append(titleText);

    QGraphicsRectItem* player1Background = new QGraphicsRectItem(350, 300, 300, 100);
    player1Background->setBrush(QBrush(Qt::white));
    addToScene(player1Background);
    listG.append(player1Background);

    QGraphicsRectItem* player2Background = new QGraphicsRectItem(350, 450, 300, 100);
    player2Background->setBrush(QBrush(Qt::white));
    addToScene(player2Background);
    listG.append(player2Background);

    QGraphicsPixmapItem *p = new QGraphicsPixmapItem();
    p->setPixmap(QPixmap(":/images/king1.png"));
    p->setPos(575,320);
    addToScene(p);
    listG.append(p);

    QGraphicsPixmapItem *p1 = new QGraphicsPixmapItem();
    p1->setPixmap(QPixmap(":/images/king.png"));
    p1->setPos(575,470);
    addToScene(p1);
    listG.append(p1);

    QFont playerFont("Arial", 16);

    QGraphicsTextItem *player1Name = new QGraphicsTextItem("");
    player1Name->setFont(playerFont);
    player1Name->setDefaultTextColor(Qt::red);
    player1Name->setPos(400, 315);
    addToScene(player1Name);
    listG.append(player1Name);

    QGraphicsTextItem *player1Elo = new QGraphicsTextItem("");
    player1Elo->setFont(playerFont);
    player1Elo->setDefaultTextColor(Qt::black);
    player1Elo->setPos(400, 355);
    addToScene(player1Elo);
    listG.append(player1Elo);

    QGraphicsTextItem *player2Name = new QGraphicsTextItem("");
    player2Name->setFont(playerFont);
    player2Name->setDefaultTextColor(Qt::black);
    player2Name->setPos(400, 465);
    addToScene(player2Name);
    listG.append(player2Name);

    QGraphicsTextItem *player2Elo = new QGraphicsTextItem("");
    player2Elo->setFont(playerFont);
    player2Elo->setDefaultTextColor(Qt::black);
    player2Elo->setPos(400, 505);
    addToScene(player2Elo);
    listG.append(player2Elo);

    QGraphicsRectItem* lockBackground = new QGraphicsRectItem(350, 450, 300, 100);
    lockBackground->setBrush(QBrush(Qt::black));
    lockBackground->setOpacity(0);
    addToScene(lockBackground);
    listG.append(lockBackground);

    QFont waitingFont("Arial", 36);

    QGraphicsTextItem *waitingText = new QGraphicsTextItem("");
    waitingText->setFont(waitingFont);
    waitingText->setDefaultTextColor(Qt::white);
    waitingText->setPos(500, 475);
    addToScene(waitingText);
    listG.append(waitingText);

    if (player.getName().isEmpty() && player.getUsername().isEmpty()) {
        player1Name->setPlainText(user->getName());
        QString player1EloText = QString("Elo: %1").arg(user->getElo());
        player1Elo->setPlainText(player1EloText);
        lockBackground->setOpacity(1);
        waitingText->setPlainText("NONE");
        waitingText->setPos(500 - waitingText->boundingRect().width()/2, 475);
    } else {
        player1Name->setPlainText(player.getName());
        QString player1EloText = QString("Elo: %1").arg(player.getElo());
        player1Elo->setPlainText(player1EloText);

        player2Name->setPlainText(user->getName());
        QString player2EloText = QString("Elo: %1").arg(user->getElo());
        player2Elo->setPlainText(player2EloText);

        lockBackground->setOpacity(0);
        waitingText->setPlainText("");
        waitingText->setPos(500 - waitingText->boundingRect().width()/2, 475);
    }

    QGraphicsRectItem* listPlayerBackground = new QGraphicsRectItem(750, 299, 325, 342);
    listPlayerBackground->setBrush(QBrush(Qt::white));
    addToScene(listPlayerBackground);
    listG.append(listPlayerBackground);

    // Nút Start (disabled ban đầu)
    Button *startButton = new Button("Start");
    startButton->setPos(400, 600);
    startButton->setEnabled(false);
    connect(startButton, &Button::clicked, this, []() {
        qDebug() << "Game started!";
    });
    addToScene(startButton);
    listG.append(startButton);

    Button *outButton = new Button("Out");
    outButton->setPos(0, 0);
    addToScene(outButton);
    listG.append(outButton);
    connect(outButton, &Button::clicked, this, [=]() mutable {
        disconnect(clientManager, &ClientManager::getListPlayerResult, this, nullptr);
        displayMenu("");
    });

    connect(clientManager, &ClientManager::getListPlayerResult, this, [=](const QString &status, const QString &message, const QList<Player> &playerList) {
        if (status == "success") {
            qDeleteAll(players);
            players.clear();
            for (const Player &player : playerList) {
                players.append(new Player(player));
            }

            for (QGraphicsItem *item : listPlayerItems) {
                removeFromScene(item);
                delete item;
            }
            listPlayerItems.clear();

            createPlayerListView(players);
        } else if (status == "failure") {
            qDebug() << message;
        }
    });
}

void Game::createPlayerListView(const QList<Player *> &playerList) {
    QGraphicsScene *listPlayerScene = new QGraphicsScene();
    int yOffset = 0;

    for (Player *player : playerList) {
        QGraphicsItemGroup *playerGroup = new QGraphicsItemGroup();

        QGraphicsRectItem *background = new QGraphicsRectItem(0, yOffset, 275, 75);
        background->setBrush(QBrush(QColor(238, 238, 238)));
        background->setPen(Qt::NoPen);
        playerGroup->addToGroup(background);

        QGraphicsTextItem *nameItem = new QGraphicsTextItem(player->getName(), background);
        nameItem->setFont(QFont("Arial", 20, QFont::Bold));
        nameItem->setDefaultTextColor(Qt::black);
        nameItem->setPos(10, yOffset + 5);
        playerGroup->addToGroup(nameItem);

        QString eloInfo = "Elo: " + QString::number(player->getElo());
        QGraphicsTextItem *eloItem = new QGraphicsTextItem(eloInfo, background);
        eloItem->setFont(QFont("Arial", 15));
        eloItem->setDefaultTextColor(Qt::black);
        eloItem->setPos(15, yOffset + 45);
        playerGroup->addToGroup(eloItem);

        QGraphicsTextItem *statusItem = new QGraphicsTextItem(player->getStatus(), background);
        statusItem->setFont(QFont("Arial", 12));
        if (player->getStatus() == "Online") {
            statusItem->setDefaultTextColor(Qt::green);
        } else if (player->getStatus() == "In Match") {
            statusItem->setDefaultTextColor(Qt::red);
        } else {
            statusItem->setDefaultTextColor(Qt::yellow);
        }
        statusItem->setPos(150, yOffset + 30);
        playerGroup->addToGroup(statusItem);

        listPlayerScene->addItem(playerGroup);

        if (player->getStatus() == "Online") {
            Button *addButton = new Button("+");
            addButton->setRect(230, yOffset + 22.5, 30, 30);
            connect(addButton, &Button::clicked, this, [player]() {
                qDebug() << "Added player:" << player->getName();
                if (clientManager) {
                    clientManager->sendInvitePlayerRequest(user->getUsername(), user->getName(), player->getUsername());
                }
            });
            listPlayerScene->addItem(addButton);
        }

        yOffset += 85;
    }

    QGraphicsView *listPlayerView = new QGraphicsView();
    listPlayerView->setScene(listPlayerScene);
    listPlayerView->setFixedSize(325, 340);
    listPlayerView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listPlayerView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listPlayerView->setStyleSheet("background-color: transparent; border: none;");
    listPlayerView->setFrameStyle(QFrame::NoFrame);

    QGraphicsProxyWidget *proxyWidget = new QGraphicsProxyWidget();
    proxyWidget->setWidget(listPlayerView);
    proxyWidget->setPos(750, 300);
    addToScene(proxyWidget);
    listPlayerItems.append(proxyWidget);
}

extern ClientManager *clientManager;
extern User *user;
extern QList<Player*> players;
void Game::displayProfile()
{
    bool isEdit = false;
    bool isChangePassword = false;
    QString oldName = user->getName();
    QString oldPassword = user->getPassword();

    // Xóa các mục hiện tại
    clearScene();

    // Tiêu đề
    QGraphicsTextItem *profileTitle = new QGraphicsTextItem("Profile");
    QFont profileFont("arial", 40);
    profileTitle->setFont(profileFont);
    profileTitle->setPos(width()/2 - profileTitle->boundingRect().width()/2, 100);
    addToScene(profileTitle);
    listG.append(profileTitle);
\
    QFont titleFont("arial", 20);

    QGraphicsTextItem *nameTitle = new QGraphicsTextItem("Name");
    // nameTitle->setFont(titleFont);
    nameTitle->setPos(500, 200);
    addToScene(nameTitle);
    listG.append(nameTitle);

    QGraphicsTextItem *eloTitle = new QGraphicsTextItem("Elo");
    // eloTitle->setFont(titleFont);
    eloTitle->setPos(500, 300);
    addToScene(eloTitle);
    listG.append(eloTitle);

    QGraphicsTextItem *passwordTitle = new QGraphicsTextItem("Password");
    // passwordTitle->setFont(titleFont);
    passwordTitle->setPos(500, 400);
    addToScene(passwordTitle);
    listG.append(passwordTitle);

    QGraphicsTextItem *newPasswordTitle = new QGraphicsTextItem("New password");
    // newPasswordTitle->setFont(titleFont);
    newPasswordTitle->setPos(500, 500);
    newPasswordTitle->hide();
    addToScene(newPasswordTitle);
    listG.append(newPasswordTitle);

    qDebug()<<user;

    QLineEdit *nameInput = new QLineEdit();
    nameInput->setText(user->getName());
    nameInput->setPlaceholderText("Enter your name");
    nameInput->setFixedWidth(250);
    QGraphicsProxyWidget *proxyName = gameScene->addWidget(nameInput);
    proxyName->setPos(650, 200);
    proxyName->setEnabled(isEdit);
    listG.append(proxyName);

    QLineEdit *eloInput = new QLineEdit();
    qDebug() << user->getElo();
    eloInput->setText(QString::number(user->getElo()));
    eloInput->setFixedWidth(250);
    QGraphicsProxyWidget *proxyElo = gameScene->addWidget(eloInput);
    proxyElo->setPos(650, 300);
    proxyElo->setEnabled(false);
    listG.append(proxyElo);

    QLineEdit *passwordInput = new QLineEdit();
    passwordInput->setText(user->getPassword());
    passwordInput->setPlaceholderText("Enter password");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setFixedWidth(250);
    QGraphicsProxyWidget *proxyPassword = gameScene->addWidget(passwordInput);
    proxyPassword->setPos(650, 400);
    proxyPassword->setEnabled(false);
    listG.append(proxyPassword);

    QLineEdit *newPasswordInput = new QLineEdit();
    newPasswordInput->setPlaceholderText("Enter new password");
    newPasswordInput->setEchoMode(QLineEdit::Password);
    newPasswordInput->setFixedWidth(250);
    QGraphicsProxyWidget *proxyNewPassword = gameScene->addWidget(newPasswordInput);
    proxyNewPassword->hide();
    proxyNewPassword->setPos(650, 500);
    listG.append(proxyNewPassword);

    Button *editButton = new Button("Edit");
    editButton->setPos(600, 600);

    Button *changePasswordButton = new Button("Change Password");
    changePasswordButton->setPos(600, 700);

    connect(editButton, &Button::clicked, this, [=]() mutable {
        isEdit = !isEdit;
        qDebug() << "isEdit:" << isEdit;

        proxyName->setEnabled(isEdit);
        if (isEdit) {
            editButton->setText("Save");
            changePasswordButton->hide();
        } else {
            if (clientManager) {
                clientManager->sendUpdateProfileRequest(user->getUsername(), nameInput->text());
            }
            editButton->setText("Edit");
            changePasswordButton->show();
        }
    });
    addToScene(editButton);
    listG.append(editButton);

    connect(changePasswordButton, &Button::clicked, this, [=]() mutable {
        if (isChangePassword && passwordInput->text() != newPasswordInput->text()) {
            qDebug() << "mat khau khong trung khop";
            return;
        }
        isChangePassword = !isChangePassword;
        qDebug() << "isChangePassword:" << isChangePassword;

        proxyPassword->setEnabled(isChangePassword);
        if (isChangePassword) {
            passwordInput->setText("");
            newPasswordInput->setText("");
            changePasswordButton->setText("Save");
            editButton->hide();
            proxyNewPassword->show();
            newPasswordTitle->show();
        } else {
            if (clientManager) {
                clientManager->sendChangePasswordRequest(user->getUsername(), newPasswordInput->text());
            }
            changePasswordButton->setText("Change Password");
            editButton->show();
            proxyNewPassword->hide();
            newPasswordTitle->hide();
        }
    });
    addToScene(changePasswordButton);
    listG.append(changePasswordButton);

    QGraphicsTextItem *errorText = new QGraphicsTextItem("");
    errorText->setDefaultTextColor(Qt::red);
    addToScene(errorText);
    listG.append(errorText);

    connect(clientManager, &ClientManager::updateProfileResult, this, [=](const QString &status, const QString &message) {
        if (status == "success") {
            user->setName(nameInput->text());
            displayProfile();
        } else if (status == "failure") {
            nameInput->setText(user->getName());
            errorText->setPlainText(message);
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 800);
        }
    });

    connect(clientManager, &ClientManager::changePasswordResult, this, [=](const QString &status, const QString &message) {
        if (status == "success") {
            user->setPassword(passwordInput->text());
            displayProfile();
        } else if (status == "failure") {
            passwordInput->setText(user->getPassword());
            errorText->setPlainText(message);
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 800);
        }
    });

    Button *outButton = new Button("Out");
    outButton->setPos(0, 0);
    addToScene(outButton);
    listG.append(outButton);
    connect(outButton, &Button::clicked, this, [=]() mutable {
        displayMenu("");
    });
}

void Game::gameOver()
{
    // removeAll();
    setTurn("WHITE");
    alivePiece.clear();
    chess->reset();




}

void Game::removeAll(){
    QList<QGraphicsItem*> itemsList = gameScene->items();
    for(size_t i = 0, n = itemsList.size();i<n;i++) {
        if(itemsList[i]!=check)
            removeFromScene(itemsList[i]);
    }
}

void Game::clearScene() {
    for (auto item : listG) {
        removeFromScene(item);
    }
    listG.clear();
}
