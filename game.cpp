#include "game.h"
#include "button.h"
#include <QPixmap>
#include "king.h"
#include <QDebug>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include "clientmanager.h"
#include "user.h"

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

void Game::drawChessBoard()
{
    chess = new ChessBoard();
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
    chess->addChessPiece(); // Thêm các quân cờ vào bàn cờ
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
void Game::displayLogin() {
    drawChessBoard();

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
    errorText->setPos(width()/2 - 50, 450);
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

        connect(clientManager, &ClientManager::loginResult, this, [=](bool success) {
            if (success) {
                displayWaitConnect();
            } else {
                qDebug() << "login fail!";
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
}

void Game::displayRegister() {
    drawChessBoard();

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
    errorText->setPos(width()/2, 450);
    addToScene(errorText);
    listG.append(errorText);

    QGraphicsTextItem *registerSuccess = new QGraphicsTextItem("");
    registerSuccess->setDefaultTextColor(Qt::green);
    registerSuccess->setPos(width()/2, 450);
    addToScene(registerSuccess);
    listG.append(registerSuccess);

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
        // Kiểm tra đăng ký và hiển thị thông báo nếu thành công
        qDebug() << "Registration Successful!";
        // Hiển thị thông báo và chuyển về màn hình login
        displayLogin();  // Quay lại màn hình login sau khi đăng ký thành công
    });
    addToScene(loginButton);
    listG.append(loginButton);

    // Kết nối tín hiệu từ ClientManager
    connect(clientManager, &ClientManager::registerResponseReceived, this, [=](const QString &status, const QString &message) {
        if (status == "success") {
            registerSuccess->setPlainText(message);
            registerSuccess->setPos(width()/2 - registerSuccess->boundingRect().width()/2, 450);
            errorText->setPlainText("");  // Xóa lỗi
        } else if (status == "failure") {
            errorText->setPlainText(message);
            errorText->setPos(width()/2 - errorText->boundingRect().width()/2, 450);
            registerSuccess->setPlainText("");  // Xóa thông báo thành công
        }
    });
}


void Game::displayWaitConnect() {
    drawChessBoard();

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Menu");
    QFont titleFont("arial", 50);
    titleText->setFont(titleFont);
    int xPos = width()/2 - titleText->boundingRect().width()/2;
    int yPos = 150;
    titleText->setPos(xPos, yPos);
    addToScene(titleText);
    listG.append(titleText);

    QGraphicsTextItem *connectingText = new QGraphicsTextItem("Connecting to server...");
    connectingText->setFont(QFont("arial", 18));
    connectingText->setPos(width() / 2 - connectingText->boundingRect().width()/2, height() / 2 - 50);
    addToScene(connectingText);
    listG.append(connectingText);

    // Gửi yêu cầu connect với token giả định (có thể thay đổi)
    if (clientManager) {
        // Set sau
        user->setToken("hihi");
        clientManager->sendConnectRequest(user->getToken());
    }

    // Kết nối tín hiệu từ ClientManager
    connect(clientManager, &ClientManager::connectionResult, this, [=](const QString &status, const QString &message) {
        if (status == "success") {
            displayMenu();
        } else if (status == "failure") {
            connectingText->setDefaultTextColor(Qt::red);
            connectingText->setPlainText(message);
        }
    });
}

void Game::displayMenu() {
    drawChessBoard();

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
    errorText->setPos(width()/2, 450);
    addToScene(errorText);
    listG.append(errorText);

    // Nút tìm trận
    Button *findMatchButton = new Button("Find Match");
    int btnXPos = width()/2 - findMatchButton->boundingRect().width()/2;
    int btnYPos = 400;
    findMatchButton->setPos(btnXPos, btnYPos);
    // Gửi yêu cầu connect với token giả định (có thể thay đổi)
    if (clientManager) {
        // Set sau
        user->setToken("hihi");
        clientManager->sendConnectRequest(user->getToken());
    }

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
}

void Game::displayRoom(const QString &room) {
    drawChessBoard();

    clearScene();

    QGraphicsTextItem *titleText = new QGraphicsTextItem("Room: " + room);
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

    // Hiển thị danh sách chờ
    QGraphicsTextItem *listPlayerLabel = new QGraphicsTextItem("List Player:");
    listPlayerLabel->setDefaultTextColor(Qt::black);
    listPlayerLabel->setFont(playerFont);
    listPlayerLabel->setPos(800, 250);
    addToScene(listPlayerLabel);
    listG.append(listPlayerLabel);

    // Nút Start (disabled ban đầu)
    Button *startButton = new Button("Start");
    startButton->setPos(400, 600);
    startButton->setEnabled(false);
    connect(startButton, &Button::clicked, this, []() {
        qDebug() << "Game started!";
    });
    addToScene(startButton);
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
