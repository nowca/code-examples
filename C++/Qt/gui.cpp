#include "modules/gui.h"
#include <QDebug>

Gui* Gui::instance          = ZERO;

/* program configuration */
bool Config::DEBUG          = OFF;
bool Config::computer       = ON;
bool Config::side           = WHITE;
int  Config::difficulty     = 3;
bool Config::sound          = ON;
bool Config::fullscreen     = OFF;
bool Config::showLegalMoves = ON;
bool Config::clock          = ON;
int  Config::time           = 15;
int  Config::theme          = 1;


Gui::Gui(QWidget *parent) : QMainWindow(parent)
{
    if ( this->checkRes() )
    {
        this -> initialize  ();
        this -> configure   ();
        this -> setup       ();
        this -> connect     ();
        this -> execute     ();
    }
    else
    {
        exit( EXIT_FAILURE );
    }
}


/**
 * @brief MainWindow::initialize
 * initializes the ui, the engine and the game board
 */
void Gui::initialize()
{
    if ( Config::DEBUG )
    {
        qDebug()<<"[MAIN_THREAD(" << QThread::currentThreadId() << ")] started -> Gui::instance";
        this -> commandLine = new QLineEdit();
    }

    this -> createCoreThread();
    this -> createMenu();

    this -> centralWidget       = new QWidget();
    this -> gridLayout          = new QGridLayout();
    this -> infoText            = new QString();
    this -> infoView            = new QTextEdit();
    this -> infoCursor          = new QTextCursor( this->infoView->textCursor() );
    this -> undoButton          = new QPushButton( tr("Undo") );
    this -> board               = new Board( this->engine );
}

void Gui::createCoreThread()
{
    this -> coreThread  = new QThread();
    this -> engine      = new Engine();
    this -> engine     -> moveToThread( coreThread );

    QObject::connect( this->coreThread, SIGNAL( started() ),  this->engine,     SLOT( run() ) );
    QObject::connect( this->engine,     SIGNAL( finished() ), this->coreThread, SLOT( quit() ), Qt::DirectConnection );

    this -> coreThread -> start();
}

/**
 * @brief MainWindow::configure
 * configures objects settings
 */
void Gui::configure()
{
    // game
    this -> board -> engine -> core -> max_depth       = Config::difficulty;
    this -> board -> engine -> core -> computer        = Config::computer;
    this -> board -> engine -> core -> computer_side   = !Config::side;

    // dialog
    this -> infoText    -> clear();
    this -> infoText    -> append( "<div style=\"background-color: gray; color: white;\"><h2>" + tr("Welcome to CuteChess") + "<img height=\"40\" width=\"40\" src=\"res/piece_king_white.svg\"></img>" + "</h2></div>" );
    this -> infoText    -> append( "<div style=\"background-color: lightgray;\">" + tr( "Feel free to enjoy this little student project." ) + "</div>" );
    this -> infoText    -> append( "<div></div><br>" );
    this -> infoView    -> setReadOnly(true);
    this -> infoView    -> setText( *(this -> infoText) );
    this -> infoView    -> setStyleSheet( "QTextEdit{color: black; background: #eeeeee; font: Courier}" );
    this -> infoView    -> viewport() -> setCursor(Qt::ArrowCursor);
    this -> infoView    -> setContextMenuPolicy(Qt::NoContextMenu);
    this -> infoView    -> setTextInteractionFlags(Qt::NoTextInteraction);
    this -> infoView    -> setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    if ( Config::DEBUG ) { this -> commandLine -> setStyleSheet( "QLineEdit{color: white; background: black;}" ); }

    this -> undoButton  -> setEnabled(false);
    this -> undoAct     -> setEnabled(false);

    if ( Config::computer ) { difficultyMenu -> setEnabled( true ); }
    else                    { difficultyMenu -> setEnabled( false ); }
}


/**
 * @brief MainWindow::setup
 * sets the window layout
 */
void Gui::setup()
{
    // set layout
    this -> gridLayout -> addWidget( this->board->graphicsView, 1, 0, 2, 1 );
    this -> gridLayout -> addWidget( this->infoView,            1, 1, 1, 2 );
    this -> gridLayout -> addWidget( this->undoButton,          2, 1, 1, 2 );
    this -> gridLayout -> setColumnMinimumWidth(2, 300);

    if ( Config::DEBUG ) { gridLayout -> addWidget( this->commandLine, 3, 1, 1, 2 ); }

    this -> centralWidget -> setLayout( this->gridLayout );
    setCentralWidget( this->centralWidget );

    // create statusbar
    statusBar() -> showMessage( tr( " http://www.htw-berlin.de" ) );

    // window
    setWindowTitle( "CuteChess" );
    setMinimumSize( 1000, 640 );
    resize( 1000, 640 );
}


/**
 * @brief MainWindow::connect
 * creates connections between signals and slots
 */
void Gui::connect ()
{
    // squares
    for (int i = 0; i < 64; ++i)
    {
        QObject::connect( this -> board -> fieldGraphics[i % 8][i / 8] , &Field::mousePressEvent, [this, i]() { this -> board -> setActiveField( this->board->fieldGraphics[i % 8][i / 8] ); });
    }

    // undo button
    QObject::connect( this -> undoButton, &QPushButton::clicked, this, &Gui::menu_undo );

    // commandLine
    if ( Config::DEBUG ) { QObject::connect( this -> commandLine, &QLineEdit::returnPressed, this, &Gui::sendCmd ); }

    // menu
    QObject::connect(newAct,        SIGNAL(triggered()), this, SLOT(menu_new()));
    QObject::connect(exitAct,       SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(undoAct,       SIGNAL(triggered()), this, SLOT(menu_undo()));
    QObject::connect(fullscreenAct, SIGNAL(triggered()), this, SLOT(menu_fullscreen()));
    QObject::connect(soundAct,      SIGNAL(triggered()), this, SLOT(menu_sound()));
    QObject::connect(showMovesAct,  SIGNAL(triggered()), this, SLOT(menu_show_moves()));
    QObject::connect(aboutAct,      SIGNAL(triggered()), this, SLOT(menu_about()));
    QObject::connect(diff_1_Act,    SIGNAL(triggered()), this, SLOT(menu_diff_1()));
    QObject::connect(diff_2_Act,    SIGNAL(triggered()), this, SLOT(menu_diff_2()));
    QObject::connect(diff_3_Act,    SIGNAL(triggered()), this, SLOT(menu_diff_3()));
    QObject::connect(diff_4_Act,    SIGNAL(triggered()), this, SLOT(menu_diff_4()));
    QObject::connect(diff_5_Act,    SIGNAL(triggered()), this, SLOT(menu_diff_5()));
}


/**
 * @brief Gui::execute
 * starts initializes start commands
 */
void Gui::execute ()
{
    this -> showMaximized();
    this -> resizeWindow();
    Config::loadSettings();
    this -> setOptions();
    this -> start();
}


/**
 * @brief Gui::getInstance
 * @return a new instance of Gui::ui if it hasn't been set already
 */
Gui* Gui::getInstance()
{
    if ( !Gui::instance ) { Gui::instance = new Gui(); }
    return Gui::instance;
}


/**
 * @brief Gui::checkResources
 * Checks the existence of the media files of the program
 * @return true if all files found
 */
bool Gui::checkRes()
{
    QStringList resources = {
        "./res/background.svg",
        "./res/board_classic.svg",
        "./res/board_wood_brown.svg",
        "./res/board_wood_grey.svg",
        "./res/piece_bishop_black.svg",
        "./res/piece_bishop_white.svg",
        "./res/piece_king_black.svg",
        "./res/piece_king_white.svg",
        "./res/piece_knight_black.svg",
        "./res/piece_knight_white.svg",
        "./res/piece_pawn_black.svg",
        "./res/piece_pawn_white.svg",
        "./res/piece_queen_black.svg",
        "./res/piece_queen_white.svg",
        "./res/piece_rook_black.svg",
        "./res/piece_rook_white.svg",
        "./res/sound_end.wav",
        "./res/sound_error.wav",
        "./res/sound_hit.wav",
        "./res/sound_start.wav"
    };




    foreach (const QString &file_path, resources)
    {
        QFile file( file_path );
        if ( !file.exists() ) { QMessageBox::critical(this, "Error", "<b>File not found:</b><br><br>" + file_path); return 0; }
    }

    return 1;
}


/**
 * @brief MainWindow::refresh
 * refreshes ui dialog and sets elements
 */
void Gui::refresh()
{
    this -> infoScrollbar  = this -> infoView -> verticalScrollBar();
    this -> infoScrollbar -> setValue( this->infoScrollbar->maximum() );

    qApp -> processEvents();

    if ( (this->board->playing) && (this->board->engine->core->hdp == 0) )
    {
        this -> undoButton  -> setEnabled(false);
        this -> undoAct     -> setEnabled(false);
    }
    else if ( !(this->engine->working) && this->board->playing )
    {
        this -> undoButton -> setEnabled(true);
        this -> undoAct    -> setEnabled(true);
    }
}


/**
 * @brief Gui::setOptions
 * sets all settings
 */
void Gui::setOptions()
{
    if ( Config::fullscreen )
    {
        this -> fullscreenAct -> setChecked(true);
        this -> menu_fullscreen();
    }

    if ( Config::sound )
    {
        this -> soundAct -> setChecked(true);
        this -> menu_sound();
    }

    if ( Config::showLegalMoves )
    {
        this -> showMovesAct -> setChecked(true);
        this -> menu_show_moves();
    }

    switch ( Config::difficulty )
    {
    case 1:
        this -> diff_1_Act -> setChecked( true );
        this -> menu_diff_1();
        break;
    case 2:
        this -> diff_2_Act -> setChecked( true );
        this -> menu_diff_2();
        break;
    case 3:
        this -> diff_3_Act -> setChecked( true );
        this -> menu_diff_3();
        break;
    case 4:
        this -> diff_4_Act -> setChecked( true );
        this -> menu_diff_4();
        break;
    case 5:
        this -> diff_5_Act -> setChecked( true );
        this -> menu_diff_5();
        break;
    }
}

/**
 * @brief Gui::start
 * some game start commands
 */
void Gui::start()
{
    this -> board -> playing = true;

    // color black ? -> computer begins
    if ( this->board->engine->core->computer && this->board->engine->core->computer_side == WHITE )
    {
        //this -> board->core  -> run( (char*)"on" );
        this -> engine -> moveCPU();
        while (this->engine->working) {}

        this -> board -> updateView();
        this -> printLastMove();
    }

     if ( this->board->playing && Config::sound) { QSound::play( "./res/sound_start.wav" ); }
}


/**
 * @brief Gui::resizeEvent
 * calls window resizing method
 * @param event
 */
void Gui::resizeEvent ( QResizeEvent *event )
{
    (void)(event);
    this -> resizeWindow();
}


/**
 * @brief Gui::resizeWindow
 * rescales the graphicsview, if window is resized
 */
void Gui::resizeWindow ()
{
    QRectF bounds = this -> board -> graphicsScene -> itemsBoundingRect();
    this -> board -> graphicsView -> fitInView( bounds, Qt::KeepAspectRatio );
}


/**
 * @brief Gui::sendCmd
 * sends an command char array from the debug command line to the core.
 */
void Gui::sendCmd ()
{
    if ( Config::DEBUG )
    {
        if ( !QString::compare( this->commandLine->text(), "on" ) )
        {
            //this -> engine -> moveCPU();
            //this -> engine -> core        -> run( (char*)this->commandLine->text().toStdString().c_str() );
        }
        else if ( !QString::compare( this->commandLine->text(), "d" ) )
        {
            this -> engine -> print();
        }
        else
        {
            //this -> engine -> core        -> run( (char*)this->commandLine->text().toStdString().c_str() );
        }

        //this ->engine->moveCPU();
        //this -> board -> upd
        //this->engine->print();
        this -> commandLine -> setText( "" );
        this -> commandLine -> clear();

        if ( QString::compare( this->commandLine->text(), "on" ) )
        {
            this -> board -> updateView();
            //this -> printLastMove();
        }
    }
}


/**
 * @brief Gui::printInfo
 * prints a string in the info view
 * @param inputString string that shall be appended to the info view
 */
void Gui::printInfo ( QString inputString )
{
    QString infoString = this -> infoView -> toHtml();
    this -> infoCursor -> insertHtml( inputString );
    this -> infoCursor -> movePosition( QTextCursor::End );
}


/**
 * @brief Gui::printLastMove
 * prints the last move in the history
 */
void Gui::printLastMove ()
{
    int     *hdp            = &(this -> board -> engine -> core -> hdp);

    int     piece           = this -> board -> engine -> core -> piece[this->board->engine->core->hist[*hdp-1].m.dest];
    int     color           = this -> board -> engine -> core -> color[this->board->engine->core->hist[*hdp-1].m.dest];
    int     from            = this -> board -> engine -> core -> hist[*hdp-1].m.from;
    int     dest            = this -> board -> engine -> core -> hist[*hdp-1].m.dest;
    int     total           = *hdp;

    char    from_alpha      = 97 + (from % 8);
    int     from_num        =  8 - (from / 8);
    char    dest_alpha      = 97 + (dest % 8);
    int     dest_num        =  8 - (dest / 8);


    QString seperator       = "-";
    if ( this->board->engine->core->hist[*hdp-1].cap != 7 ) { seperator = "x"; }

    QString moveNo          = QString ("%1" ).arg( total, 3, 10, QChar('0') );
    QString move            = QString( "%2%3 " + seperator + " %4%5" ).arg( from_alpha ).arg( from_num ).arg( dest_alpha ).arg( dest_num );
    QString turnNo          = QString ("%1" ).arg( ((total/2)+1), 2, 10, QChar('0') );

    QString outputString    = "";
    QString source          = "";
    QString colorName       = "white";
    QString pieceName       = "pawn";

    if ( color == 1 ) { colorName = "black"; }

    switch ( piece )
    {
        case 0: pieceName = "pawn";     break;
        case 1: pieceName = "knight";   break;
        case 2: pieceName = "bishop";   break;
        case 3: pieceName = "rook";     break;
        case 4: pieceName = "queen";    break;
        case 5: pieceName = "king";     break;
    }

    source = "res/piece_" + pieceName + "_" + colorName + ".svg";

    if (total % 2 == 1) { outputString += "<span style=\"font-size: 16px; background-color: gray; color: lightgrey; \"><b>" + turnNo + " </b></span>&nbsp;&nbsp;"; }

    outputString += QString("<span style=\"font-family: Courier; font-size: 14px; background-color: lightgray; color: black;\"> " + moveNo + "<img height=\"24\" width=\"24\" src=\"" + source + "\"></img><b>" + move + " </b></span>");

    if (total % 2 == 1) { outputString += "<span>&nbsp;&nbsp;</span>"; }
    if (total % 2 == 0) { outputString += "<br> "; }

    printInfo( outputString );
}


/**
 * @brief Gui::contextMenuEvent
 * window menu event handler
 * @param event
 */
void Gui::contextMenuEvent( QContextMenuEvent *event )
{
    QMenu menu( this );
    menu.exec( event->globalPos() );
}


/**
 * @brief Gui::menu_new
 * start a new game
 */
void Gui::menu_new()
{
    this -> board -> boardFog -> setOpacity( 0.7 );

    Dialog *newGameDialog = new Dialog(this);
    newGameDialog -> setWindowModality( Qt::WindowModal );
    newGameDialog -> setWindowFlags( Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint );
    while ( newGameDialog->exec() ) {}

    this -> engine -> reset();
    while ( this->engine->resetting ) {};

    this -> board -> setCore(this->engine->core);

    this -> board -> rescalePieces( false );
    this -> board -> unsetAllFields();
    this -> board -> updateView();

    this -> board -> boardFog -> setOpacity( 0 );

    this -> configure();
    this -> start();
}


/**
 * @brief Gui::menu_undo
 * undo the last move
 */
void Gui::menu_undo()
{
    this -> board -> undo();

    int ctr = 13;

    if ( Config::computer )                           { ctr = 33; }
    else if (this->board->engine->core->hdp % 2 == 0) { ctr = 20; }

    for (int i = 0; i < ctr; i++) { this -> infoCursor -> deletePreviousChar(); }
}


/**
 * @brief Gui::menu_fullscreen
 * toggles fullscreen and windowed mode
 */
void Gui::menu_fullscreen()
{
    if ( fullscreenAct->isChecked() )
    {
        this -> showFullScreen();
        Config::fullscreen = 1;
    }
    else
    {
        this -> showNormal();
        Config::fullscreen = 0;
    }

    Config::saveSettings();
}


void Gui::menu_sound()
{
    if ( soundAct->isChecked() ) { Config::sound = 1; }
    else                         { Config::sound = 0; }

    Config::saveSettings();
}


/**
 * @brief Gui::menu_show_moves
 * sets move field highlighting
 */
void Gui::menu_show_moves()
{
    if ( showMovesAct->isChecked() ) { Config::showLegalMoves = 1; }
    else
    {
        this -> board -> unsetAllFields();
        Config::showLegalMoves = 0;
    }

    Config::saveSettings();
}


/**
 * @brief Gui::menu_diff_1
 * sets difficulty level to 1
 */
void Gui::menu_diff_1()
{
    Config::difficulty    = 1;
    this -> board -> engine -> core -> max_depth = 1;
    Config::saveSettings();
}

/**
 * @brief Gui::menu_diff_2
 * sets difficulty level to 2
 */
void Gui::menu_diff_2()
{
    Config::difficulty    = 2;
    this -> board -> engine -> core -> max_depth = 2;
    Config::saveSettings();
}

/**
 * @brief Gui::menu_diff_3
 * sets difficulty level to 3
 */
void Gui::menu_diff_3()
{
    Config::difficulty = 3;
    this -> board -> engine -> core -> max_depth = 3;
    Config::saveSettings();
}

/**
 * @brief Gui::menu_diff_4
 * sets difficulty level to 4
 */
void Gui::menu_diff_4()
{
    Config::difficulty = 4;
    this -> board -> engine -> core -> max_depth = 4;
    Config::saveSettings();
}

/**
 * @brief Gui::menu_diff_5
 * sets difficulty level to 5
 */
void Gui::menu_diff_5()
{
    Config::difficulty = 5;
    this -> board -> engine -> core -> max_depth = 5;
    Config::saveSettings();
}

/**
 * @brief Gui::menu_about
 * show about info dialog
 */
void Gui::menu_about()
{
    QMessageBox::about(this, tr("About CuteChess"),
            tr("<b>CuteChess</b> is a a student project by Jonathan Nowca and Simon Mühlchen<br>"
               "<br>"
               "This program is using the Qt4-Interface and secondchess engine by Emilio Díaz.<br>"
               "<br>"
               "<a href=\"http://qt-project.org\">http://qt-project.org</a><br>"
               "<a href=\"http://github.com/emdio/secondchess\">http://github.com/emdio/secondchess</a><br>"));
}


/**
 * @brief Gui::createMenu
 * helper method that creates the menus
 */
void Gui::createMenu()
 {
    newAct          =  new QAction( tr( "&New" ), this );
    newAct          -> setShortcuts( QKeySequence::New );
    newAct          -> setStatusTip( tr( "Start a new game" ) );

    exitAct         =  new QAction( tr( "E&xit" ), this );
    exitAct         -> setShortcuts( QKeySequence::Quit );
    exitAct         -> setStatusTip( tr( "Exit the game" ) );

    undoAct         =  new QAction( tr( "&Undo" ), this );
    undoAct         -> setShortcuts( QKeySequence::Undo );
    undoAct         -> setStatusTip( tr( "Undo the last move" ) );

    fullscreenAct   =  new QAction( tr( "&Fullscreen" ), this );
    fullscreenAct   -> setCheckable( true );
    fullscreenAct   -> setShortcut( tr( "F11" ) );
    fullscreenAct   -> setStatusTip( tr( "Switch between normal mode and fullscreen mode" ) );

    soundAct         =  new QAction( tr( "S&ound" ), this );
    soundAct         -> setCheckable( true );
    soundAct         -> setShortcut( tr( "F10" ) );
    soundAct         -> setStatusTip( tr( "Turn sound output on or off" ) );

    showMovesAct     =  new QAction( tr( "&Highlight legal moves" ), this );
    showMovesAct     -> setCheckable( true );
    showMovesAct     -> setShortcut( tr( "F9" ) );
    showMovesAct     -> setStatusTip( tr( "Highlight all possible moves by selected piece" ) );

    aboutAct        =  new QAction( tr( "&About..." ), this );
    aboutAct        -> setStatusTip( tr( "About CuteChess" ) );

    diff_1_Act      =  new QAction( tr( "1 (very fast)" ), this );
    diff_1_Act      -> setCheckable( true );
    diff_1_Act      -> setStatusTip( tr( "Set CPU difficulty level: 1 / 5" ) );

    diff_2_Act      =  new QAction( tr( "2 (fast)" ), this );
    diff_2_Act      -> setCheckable( true );
    diff_2_Act      -> setStatusTip( tr( "Set CPU difficulty level: 2 / 5" ) );

    diff_3_Act      =  new QAction( tr( "3 (normal)" ), this );
    diff_3_Act      -> setCheckable( true );
    diff_3_Act      -> setStatusTip( tr( "Set CPU difficulty level: 3 / 5" ) );

    diff_4_Act      =  new QAction( tr( "4 (slow)" ), this );
    diff_4_Act      -> setCheckable( true );
    diff_4_Act      -> setStatusTip( tr( "Set CPU difficulty level: 4 / 5" ) );

    diff_5_Act      =  new QAction( tr( "5 (very slow)" ), this );
    diff_5_Act      -> setCheckable( true );
    diff_5_Act      -> setStatusTip( tr( "Set CPU difficulty level: 5 / 5" ) );

    diffGroup       =  new QActionGroup( this );
    diffGroup       -> addAction( diff_1_Act );
    diffGroup       -> addAction( diff_2_Act );
    diffGroup       -> addAction( diff_3_Act );
    diffGroup       -> addAction( diff_4_Act );
    diffGroup       -> addAction( diff_5_Act );

    // create menus
    fileMenu        =  menuBar() -> addMenu( tr( "&Game" ) );
    fileMenu        -> addAction( newAct );
    fileMenu        -> addSeparator();
    fileMenu        -> addAction( exitAct );

    editMenu        =  menuBar() -> addMenu( tr( "&Edit" ) );
    editMenu        -> addAction( undoAct );
    editMenu        -> addSeparator();

    helpMenu        =  menuBar() -> addMenu( tr( "&Help" ) );
    helpMenu        -> addAction( aboutAct );

    optionMenu      =  editMenu-> addMenu( tr( "&Settings" ) );
    optionMenu      -> addAction( fullscreenAct );
    optionMenu      -> addAction( soundAct );
    optionMenu      -> addSeparator();
    optionMenu      -> addAction( showMovesAct );
    optionMenu      -> addSeparator();
    difficultyMenu  =  optionMenu -> addMenu( tr( "&Difficulty" ) );
    difficultyMenu  -> addAction( diff_1_Act );
    difficultyMenu  -> addAction( diff_2_Act );
    difficultyMenu  -> addAction( diff_3_Act );
    difficultyMenu  -> addAction( diff_4_Act );
    difficultyMenu  -> addAction( diff_5_Act );
}


/**
 * @brief Gui::~Gui
 * empty destructor
 */
Gui::~Gui ()
{

}
