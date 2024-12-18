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


QString Game::role = ""; // Khởi tạo giá trị mặc định

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

    // Thiết lập kích thước nền là 600x600 và đặt vị trí tại (0, 300)
    background->setPixmap(QPixmap(":/images/background.jpg").scaled(800, 900));

    background->setPos(300, 0); // Đặt vị trí của background

    background->setOpacity(0.8);

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

void Game::start(const QString &status, const QString &room, const QString &competitor, const QString &role)
{
    // Xóa các đối tượng khỏi Scene
    clearScene();
    Game::role = role; // Gán giá trị mới cho biến static

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

    // Xóa các mục đã có
    clearScene();

    // Tạo ảnh nền và tiêu đề
    QGraphicsPixmapItem *p = new QGraphicsPixmapItem();
    p->setPixmap(QPixmap(":/images/king1.png"));
    p->setPos(420,170);
    addToScene(p);
    listG.append(p);

    QGraphicsPixmapItem *p1 = new QGraphicsPixmapItem();
    p1->setPixmap(QPixmap(":/images/king.png"));
    p1->setPos(920,170);
    addToScene(p1);
    listG.append(p1);

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Chess Pro");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    int xPos = width()/2 - titleText->boundingRect().width()/2;
    int yPos = 150;
    titleText->setPos(xPos, yPos);
    addToScene(titleText);
    listG.append(titleText);

    // Tạo trường nhập tên người dùng
    QLineEdit *usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter username");
    usernameInput->setFixedWidth(200);
    QGraphicsProxyWidget *proxyUsername = gameScene->addWidget(usernameInput);
    proxyUsername->setPos(width()/2 - usernameInput->width()/2, 300);
    listG.append(proxyUsername);

    // Tạo trường nhập mật khẩu
    QLineEdit *passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter password");
    passwordInput->setEchoMode(QLineEdit::Password); // Ẩn mật khẩu
    passwordInput->setFixedWidth(200);
    QGraphicsProxyWidget *proxyPassword = gameScene->addWidget(passwordInput);
    proxyPassword->setPos(width()/2 - passwordInput->width()/2, 350);
    listG.append(proxyPassword);

    QGraphicsTextItem *errorText = new QGraphicsTextItem("");
    errorText->setDefaultTextColor(Qt::red);
    addToScene(errorText);
    listG.append(errorText);

    // Nút đăng nhập
    Button *loginButton = new Button("Login");
    int btnXPos = width()/2 - loginButton->boundingRect().width()/2;
    int btnYPos = 400;
    loginButton->setPos(btnXPos, btnYPos);
    connect(loginButton, &Button::clicked, this, [=]() {
        QString username = usernameInput->text();
        QString password = passwordInput->text();

        // Kiểm tra nếu có trường nào còn trống
        if (username.isEmpty() || password.isEmpty()) {
            qDebug() << "All fields must be filled!";
            errorText->setPlainText("All fields must be filled!");
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            return; // Dừng lại không thực hiện đăng ký
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
                qDebug() << user->getName();
                qDebug() << user->getElo();
                displayWaitConnect();
                // start("","","","WHITE");

            } else if (status == "failure") {
                errorText->setPlainText(message);
                errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            }
        });
    });
    addToScene(loginButton);
    listG.append(loginButton);

    // Nút đăng ký
    Button *registerButton = new Button("Register");
    registerButton->setPos(btnXPos, btnYPos + 100);
    connect(registerButton, &Button::clicked, this, &Game::displayRegister);
    addToScene(registerButton);
    listG.append(registerButton);

     // drawChessBoard("WHITE");
}

void Game::displayRegister() {
    // Xóa các mục hiện tại
    clearScene();

    // Tiêu đề
    QGraphicsTextItem *registerTitle = new QGraphicsTextItem("Register");
    QFont registerFont("arial", 40);
    registerTitle->setFont(registerFont);
    registerTitle->setPos(width()/2 - registerTitle->boundingRect().width()/2, 100);
    addToScene(registerTitle);
    listG.append(registerTitle);

    // Trường nhập tên
    QLineEdit *nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Enter your name");
    nameInput->setFixedWidth(200);
    QGraphicsProxyWidget *proxyName = gameScene->addWidget(nameInput);
    proxyName->setPos(width()/2 - nameInput->width()/2, 200);
    listG.append(proxyName);

    // Trường nhập tài khoản
    QLineEdit *usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter username");
    usernameInput->setFixedWidth(200);
    QGraphicsProxyWidget *proxyUsername = gameScene->addWidget(usernameInput);
    proxyUsername->setPos(width()/2 - usernameInput->width()/2, 250);
    listG.append(proxyUsername);

    // Trường nhập mật khẩu
    QLineEdit *passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Enter password");
    passwordInput->setEchoMode(QLineEdit::Password); // Ẩn mật khẩu
    passwordInput->setFixedWidth(200);
    QGraphicsProxyWidget *proxyPassword = gameScene->addWidget(passwordInput);
    proxyPassword->setPos(width()/2 - passwordInput->width()/2, 300);
    listG.append(proxyPassword);

    // Trường xác nhận mật khẩu
    QLineEdit *confirmPasswordInput = new QLineEdit();
    confirmPasswordInput->setPlaceholderText("Confirm password");
    confirmPasswordInput->setEchoMode(QLineEdit::Password); // Ẩn mật khẩu
    confirmPasswordInput->setFixedWidth(200);
    QGraphicsProxyWidget *proxyConfirmPassword = gameScene->addWidget(confirmPasswordInput);
    proxyConfirmPassword->setPos(width()/2 - confirmPasswordInput->width()/2, 350);
    listG.append(proxyConfirmPassword);

    QGraphicsTextItem *errorText = new QGraphicsTextItem("");
    errorText->setDefaultTextColor(Qt::red);
    addToScene(errorText);
    listG.append(errorText);

    QGraphicsTextItem *successText = new QGraphicsTextItem("");
    successText->setDefaultTextColor(Qt::green);
    addToScene(successText);
    listG.append(successText);

    // Nút đăng ký
    Button *registerButton = new Button("Register");
    registerButton->setPos(width()/2 - registerButton->boundingRect().width()/2, 400);
    connect(registerButton, &Button::clicked, this, [=]() {
        QString name = nameInput->text();
        QString username = usernameInput->text();
        QString password = passwordInput->text();
        QString confirmPassword = confirmPasswordInput->text();

        // Kiểm tra nếu có trường nào còn trống
        if (name.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
            qDebug() << "All fields must be filled!";
            errorText->setPlainText("All fields must be filled!");
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            return; // Dừng lại không thực hiện đăng ký
        }

        // Kiểm tra mật khẩu xác nhận
        if (password != confirmPassword) {
            qDebug() << "Passwords do not match!";
            errorText->setPlainText("Passwords do not match!");
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            return; // Dừng lại không thực hiện đăng ký
        }

        // Gọi phương thức gửi yêu cầu đăng ký đến server
        if (clientManager) {
            clientManager->sendRegisterRequest(name, username, password);
        }
    });
    addToScene(registerButton);
    listG.append(registerButton);

    // Quay lại màn hình đăng nhập
    Button *loginButton = new Button("Back to Login");
    loginButton->setPos(width()/2 - loginButton->boundingRect().width()/2, 500);
    connect(loginButton, &Button::clicked, this, [=]() {
        // Hiển thị thông báo và chuyển về màn hình login
        displayLogin();  // Quay lại màn hình login sau khi đăng ký thành công
    });
    addToScene(loginButton);
    listG.append(loginButton);

    // Kết nối tín hiệu từ ClientManager
    connect(clientManager, &ClientManager::registerResult, this, [=](const QString &status, const QString &message) {
        qDebug() << message;
        if (status == "success") {
            successText->setPlainText(message);
            successText->setPos(width()/2 - successText->boundingRect().width()/2, 450);
            errorText->setPlainText("");  // Xóa lỗi
        } else if (status == "failure") {
            errorText->setPlainText(message);
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            successText->setPlainText("");  // Xóa thông báo thành công
        }
    });
}

extern ClientManager *clientManager;
extern User *user;
extern QList<Player*> players;
void Game::displayWaitConnect() {

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("WaitConnect");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    int xPos = width()/2 - titleText->boundingRect().width()/2;
    int yPos = 150;
    titleText->setPos(xPos, yPos);
    addToScene(titleText);
    listG.append(titleText);

    QGraphicsTextItem *errorText = new QGraphicsTextItem("");
    errorText->setDefaultTextColor(Qt::red);
    addToScene(errorText);
    listG.append(errorText);

    // Gửi yêu cầu connect với token giả định (có thể thay đổi)
    if (clientManager) {
        clientManager->sendConnectRequest(user->getToken());
    }

    // Kết nối tín hiệu từ ClientManager
    connect(clientManager, &ClientManager::connectionResult, this, [=](const QString &status, const QString &message) {
        if (status == "success") {
            displayMenu();
        } else if (status == "failure") {
            errorText->setPlainText(message);
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);

            Button *loginButton = new Button("Back to Login");
            loginButton->setPos(width()/2 - loginButton->boundingRect().width()/2, 500);
            connect(loginButton, &Button::clicked, this, [=]() {
                displayLogin();
            });
            addToScene(loginButton);
            listG.append(loginButton);
        }
    });
}

void Game::displayMenu() {
    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Chess Pro");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    int xPos = width()/2 - titleText->boundingRect().width()/2;
    int yPos = 150;
    titleText->setPos(xPos, yPos);
    addToScene(titleText);
    listG.append(titleText);

    QGraphicsTextItem *errorText = new QGraphicsTextItem("");
    errorText->setDefaultTextColor(Qt::red);
    errorText->setPos(width()/2, 650);
    addToScene(errorText);
    listG.append(errorText);

    // Nút tìm trận
    Button *findMatchButton = new Button("Find Match");
    int btnXPos = width()/2 - findMatchButton->boundingRect().width()/2;
    int btnYPos = 300;
    findMatchButton->setPos(btnXPos, btnYPos);

    connect(findMatchButton, &Button::clicked, this, [=]() {
        if (clientManager) {
            clientManager->sendFindMatchRequest(user->getUsername());
        }
    });

    // Kết nối tín hiệu từ ClientManager
    connect(clientManager, &ClientManager::findMatchResult, this, [=](const QString &status, const QString &room, const QString &competitor, const QString &role) {
        if (status == "success") {
            start(status, room, competitor, role);
        } else if (status == "failure") {
            errorText->setPlainText("Error: Cannot find a match.");
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            return;
        }
    });
    addToScene(findMatchButton);
    listG.append(findMatchButton);

    // Nút tạo phòng
    Button *createRoomButton = new Button("Create Room");
    createRoomButton->setPos(btnXPos, btnYPos + 100);
    connect(createRoomButton, &Button::clicked, this, [=]() {
        if (clientManager) {
            clientManager->sendCreateRoomRequest(user->getUsername());
        }
    });
    addToScene(createRoomButton);
    listG.append(createRoomButton);

    connect(clientManager, &ClientManager::findMatchResult, this, [=](const QString &status, const QString &room) {
        if (status == "success") {
            displayRoom(room);
        } else if (status == "failure") {
            errorText->setPlainText("Error: Cannot create a room.");
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
        }
    });

    // Nút quản lý người dùng
    Button *openProfileButton = new Button("Open Profile");
    openProfileButton->setPos(btnXPos, btnYPos + 200);
    connect(openProfileButton, &Button::clicked, this, [=]() {
        displayProfile();
    });
    addToScene(openProfileButton);
    listG.append(openProfileButton);
}

void Game::displayRoom(const QString &roomID) {
    drawChessBoard("WHITE");

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Room: " + roomID);
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    int xPos = width()/2 - titleText->boundingRect().width()/2;
    int yPos = 150;
    titleText->setPos(xPos, yPos);
    addToScene(titleText);
    listG.append(titleText);

    QGraphicsTextItem *errorText = new QGraphicsTextItem("");
    errorText->setDefaultTextColor(Qt::red);
    errorText->setPos(width()/2, 450);
    addToScene(errorText);
    listG.append(errorText);

    // Hiển thị thông tin người chơi trong phòng
    QGraphicsRectItem* player1Background = new QGraphicsRectItem(350, 250, 300, 100);
    player1Background->setBrush(QBrush(Qt::white));
    addToScene(player1Background);
    listG.append(player1Background);

    QGraphicsRectItem* player2Background = new QGraphicsRectItem(350, 450, 300, 100);
    player2Background->setBrush(QBrush(Qt::white));
    addToScene(player2Background);
    listG.append(player2Background);

    QFont playerFont("arial", 15);
    QGraphicsTextItem *player1Name = new QGraphicsTextItem("Player 1: Empty");
    player1Name->setFont(playerFont);
    player1Name->setDefaultTextColor(Qt::black);
    player1Name->setPos(400, 265);
    addToScene(player1Name);
    listG.append(player1Name);

    QGraphicsTextItem *player1Elo = new QGraphicsTextItem("Elo: Empty");
    player1Elo->setFont(playerFont);
    player1Elo->setDefaultTextColor(Qt::red);
    player1Elo->setPos(400, 305);
    addToScene(player1Elo);
    listG.append(player1Elo);

    QGraphicsTextItem *player2Name = new QGraphicsTextItem("Player 2: Empty");
    player2Name->setFont(playerFont);
    player2Name->setDefaultTextColor(Qt::black);
    player2Name->setPos(400, 465);
    addToScene(player2Name);
    listG.append(player2Name);

    QGraphicsTextItem *player2Elo = new QGraphicsTextItem("Elo: Empty");
    player2Elo->setFont(playerFont);
    player2Elo->setDefaultTextColor(Qt::red);
    player2Elo->setPos(400, 505);
    addToScene(player2Elo);
    listG.append(player2Elo);

    QGraphicsRectItem* listPlayerBackground = new QGraphicsRectItem(750, 249, 302, 300);
    listPlayerBackground->setBrush(QBrush(Qt::white));
    addToScene(listPlayerBackground);
    listG.append(listPlayerBackground);

    // Scene riêng cho danh sách người chơi
    QGraphicsScene *listPlayerScene = new QGraphicsScene();
    int yOffset = 0;

    for (Player *player : players) {
        // Tạo nhóm hiển thị
        QGraphicsItemGroup *playerGroup = new QGraphicsItemGroup();

        // Tạo nền màu
        QGraphicsRectItem *background = new QGraphicsRectItem(0, yOffset, 275, 50);
        background->setBrush(QBrush(QColor(238, 238, 238)));
        background->setPen(Qt::NoPen);
        playerGroup->addToGroup(background);

        QGraphicsTextItem *nameItem = new QGraphicsTextItem(player->getName(), background);
        nameItem->setFont(QFont("Arial", 16, QFont::Bold));
        nameItem->setDefaultTextColor(Qt::white);
        nameItem->setPos(10, yOffset);
        playerGroup->addToGroup(nameItem);

        // Hiển thị ELO người chơi
        QString eloInfo = QString::number(player->getElo()) + " pts";
        QGraphicsTextItem *eloItem = new QGraphicsTextItem(eloInfo, background);
        eloItem->setFont(QFont("Arial", 14));
        eloItem->setDefaultTextColor(Qt::white);
        eloItem->setPos(10, yOffset + 25);
        playerGroup->addToGroup(eloItem);

        listPlayerScene->addItem(playerGroup);

        // Thêm nút "+" nếu người chơi đang Online
        if (player->getStatus() == "Online") {
            Button *addButton = new Button("+");
            addButton->setRect(230, yOffset + 10, 30, 30);
            addButton->alignText(230, yOffset + 10);
            addButton->setObjectName(player->getName());
            // addButton->setPos(300, yOffset); // Vị trí nút ở bên cạnh thông tin
            connect(addButton, &Button::clicked, this, [player]() {
                qDebug() << "Added player:" << player->getName();
            });
            listPlayerScene->addItem(addButton);
        }

        yOffset += 60; // Tăng khoảng cách hiển thị giữa các người chơi
    }

    // Hiển thị danh sách người chơi trong QGraphicsView
    QGraphicsView *listPlayerView = new QGraphicsView();
    listPlayerView->setScene(listPlayerScene);
    listPlayerView->setFixedSize(300, 300); // Kích thước của view
    listPlayerView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    listPlayerView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listPlayerView->setStyleSheet("background-color: transparent; border: none;");
    listPlayerView->setFrameStyle(QFrame::NoFrame);

    QGraphicsProxyWidget *proxyWidget = new QGraphicsProxyWidget();
    proxyWidget->setWidget(listPlayerView);
    proxyWidget->setPos(750, 250);
    addToScene(proxyWidget);
    listG.append(proxyWidget);

    listPlayerView->verticalScrollBar()->setValue(0);

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
    outButton->setPos(300, 50);
    addToScene(outButton);
    listG.append(outButton);
    connect(outButton, &Button::clicked, this, &Game::displayMenu);

    QGraphicsRectItem* lockBackground = new QGraphicsRectItem(350, 450, 300, 100);
    lockBackground->setBrush(QBrush(Qt::black));
    lockBackground->setOpacity(0.8);
    addToScene(lockBackground);
    listG.append(lockBackground);
}

extern ClientManager *clientManager;
extern User *user;
extern QList<Player*> players;
void Game::displayProfile()
{
    bool isEdit = false;
    bool isChangePassword = false;

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
    connect(editButton, &Button::clicked, this, [=]() mutable {
        isEdit = !isEdit;
        qDebug() << "isEdit:" << isEdit;

        proxyName->setEnabled(isEdit);
        if (isEdit) {
            editButton->setText("Save");
        } else {
            if (clientManager) {
                clientManager->sendUpdateProfileRequest(user->getUsername(), nameInput->text());
            }
            editButton->setText("Edit");
        }
    });
    addToScene(editButton);
    listG.append(editButton);

    Button *changePasswordButton = new Button("Change Password");
    changePasswordButton->setPos(600, 700);
    connect(changePasswordButton, &Button::clicked, this, [=]() mutable {
        if (isChangePassword && passwordInput->text() != newPasswordInput->text()) {
            qDebug() << "mat khau khong trung khop";
            return;
        }
        isChangePassword = !isChangePassword;
        qDebug() << "isChangePassword:" << isChangePassword;

        proxyPassword->setEnabled(isChangePassword);
        if (isChangePassword) {
            changePasswordButton->setText("Save");
            proxyNewPassword->show();
            newPasswordTitle->show();
        } else {
            if (clientManager) {
                clientManager->sendChangePasswordRequest(user->getUsername(), newPasswordInput->text());
            }
            changePasswordButton->setText("Change Password");
            proxyNewPassword->hide();
            newPasswordTitle->hide();
        }
    });
    addToScene(changePasswordButton);
    listG.append(changePasswordButton);

    // // Trường nhập mật khẩu mới
    // QLineEdit *newPasswordInput = new QLineEdit();
    // newPasswordInput->setPlaceholderText("Enter new password");
    // newPasswordInput->setEchoMode(QLineEdit::Password); // Ẩn mật khẩu
    // newPasswordInput->setFixedWidth(200);
    // QGraphicsProxyWidget *proxyNewPassword = gameScene->addWidget(newPasswordInput);
    // proxyNewPassword->setPos(width()/2 - newPasswordInput->width()/2, 300);
    // listG.append(proxyNewPassword);

    // // Trường xác nhận mật khẩu mới
    // QLineEdit *confirmNewPasswordInput = new QLineEdit();
    // confirmNewPasswordInput->setPlaceholderText("Confirm new password");
    // confirmNewPasswordInput->setEchoMode(QLineEdit::Password); // Ẩn mật khẩu
    // confirmNewPasswordInput->setFixedWidth(200);
    // QGraphicsProxyWidget *proxyConfirmNewPassword = gameScene->addWidget(confirmNewPasswordInput);
    // proxyConfirmNewPassword->setPos(width()/2 - confirmNewPasswordInput->width()/2, 350);
    // listG.append(proxyConfirmNewPassword);
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
