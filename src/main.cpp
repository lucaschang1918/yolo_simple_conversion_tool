#include <QApplication>
#include <widget/yolocontrol.hpp>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(app_icon);
    yoloControl yoloControl;
    yoloControl.setWindowIcon(QIcon(":/naiLong.ico"));
    yoloControl.show();
    return QApplication::exec();
}
