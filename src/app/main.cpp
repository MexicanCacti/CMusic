#include <QApplication>
#include "../ui/musicplayer.h"

// Boiler plate
int main(int argc, char *argv[]) {

    // Creates QApplication object, this manages application-wide resources.
    QApplication app(argc, argv);

    // Entry point for displaying. AKA the main widget
    MusicPlayer player;
    player.show();

    // Makes QApplication enter event loop. Events gen'd & sent to widgets of app (key/mouse presses...)
    return app.exec();
}