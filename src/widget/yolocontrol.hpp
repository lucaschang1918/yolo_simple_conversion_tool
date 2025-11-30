//
// Created by lzx on 2025/8/27.
//

#ifndef YOLO_YOLOCONTROL_HPP
#define YOLO_YOLOCONTROL_HPP

#include <QMainWindow>
#include <QProcess>
#include <windows.h>
#include <processthreadsapi.h>
#include <QTimer>
#include <QTextStream>
#include <QFile>
#include <QStringConverter>  // Qt6 新增的编码转换类
QT_BEGIN_NAMESPACE
namespace Ui { class yoloControl; }
QT_END_NAMESPACE

class yoloControl : public QMainWindow {
Q_OBJECT

public:
    explicit yoloControl(QWidget *parent = nullptr);

    ~yoloControl() override;
private slots:
    void on_m_btnYoloOpen_clicked();
    void on_m_btnYamlOpen_clicked();
    void on_m_btnTrainPt_clicked();
    void on_m_btnStop_clicked();
    void on_m_btn2Onnx_clicked();
    void on_m_btn2Kmodel_clicked();
    void on_m_btnPtOpen_clicked();
    void on_m_btnValOpen_clicked();
    void on_m_btnDir2Onnx_clicked();
    void on_m_Dir2Kmodel_clicked();
    void on_m_btnDownload_clicked();
  void on_m_btnTestOnnx_clicked();
  void on_m_btnTestPt_clicked();

private:
    void on_trainProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    Ui::yoloControl *ui;
    QProcess *m_trainProcess;
    QProcess *m_convertProcess;
    QString m_allOutput;
    QString m_lastSavedPath; //记录模型路径
    QString extractScript(const QString& yoloPath, const QString& resourceName);

};


#endif //YOLO_YOLOCONTROL_HPP
