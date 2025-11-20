/********************************************************************************
** Form generated from reading UI file 'yolocontrol.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_YOLOCONTROL_H
#define UI_YOLOCONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_yoloControl
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_5;
    QSplitter *splitter_2;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout_4;
    QLabel *label;
    QLabel *label_9;
    QComboBox *m_comEvnName;
    QLabel *label_2;
    QLineEdit *m_lineYoloPath;
    QPushButton *m_btnYoloOpen;
    QLabel *label_3;
    QLineEdit *m_lineYamlPath;
    QPushButton *m_btnYamlOpen;
    QComboBox *m_comVersion;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QGridLayout *gridLayout_3;
    QLabel *label_5;
    QSpinBox *m_spinEpochs;
    QLabel *label_6;
    QSpinBox *m_spinBatchSize;
    QLabel *label_7;
    QSpinBox *m_spinImageSize;
    QLabel *label_8;
    QComboBox *m_comboDevice;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_btnTrainPt;
    QPushButton *m_btnStop;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *m_btn2Onnx;
    QPushButton *m_btn2Kmodel;
    QSpacerItem *horizontalSpacer_4;
    QSplitter *splitter;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout;
    QPushButton *m_btnDownload;
    QSpacerItem *verticalSpacer;
    QLabel *label_11;
    QGridLayout *gridLayout_2;
    QLabel *label_12;
    QLineEdit *m_linePtPath;
    QPushButton *m_btnPtOpen;
    QLabel *label_13;
    QLineEdit *m_lineValPath;
    QPushButton *m_btnValOpen;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *m_btnDir2Onnx;
    QPushButton *m_Dir2Kmodel;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *verticalSpacer_2;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_10;
    QTextEdit *m_editLog;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *yoloControl)
    {
        if (yoloControl->objectName().isEmpty())
            yoloControl->setObjectName("yoloControl");
        yoloControl->resize(758, 353);
        centralwidget = new QWidget(yoloControl);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_5 = new QHBoxLayout(centralwidget);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        splitter_2 = new QSplitter(centralwidget);
        splitter_2->setObjectName("splitter_2");
        splitter_2->setOrientation(Qt::Orientation::Horizontal);
        layoutWidget = new QWidget(splitter_2);
        layoutWidget->setObjectName("layoutWidget");
        verticalLayout_3 = new QVBoxLayout(layoutWidget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName("gridLayout_4");
        label = new QLabel(layoutWidget);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_4->addWidget(label, 0, 0, 1, 2);

        label_9 = new QLabel(layoutWidget);
        label_9->setObjectName("label_9");
        label_9->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_4->addWidget(label_9, 1, 0, 1, 2);

        m_comEvnName = new QComboBox(layoutWidget);
        m_comEvnName->addItem(QString());
        m_comEvnName->setObjectName("m_comEvnName");
        m_comEvnName->setEditable(true);

        gridLayout_4->addWidget(m_comEvnName, 1, 2, 1, 2);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_4->addWidget(label_2, 2, 0, 1, 1);

        m_lineYoloPath = new QLineEdit(layoutWidget);
        m_lineYoloPath->setObjectName("m_lineYoloPath");

        gridLayout_4->addWidget(m_lineYoloPath, 2, 1, 1, 2);

        m_btnYoloOpen = new QPushButton(layoutWidget);
        m_btnYoloOpen->setObjectName("m_btnYoloOpen");

        gridLayout_4->addWidget(m_btnYoloOpen, 2, 3, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");
        label_3->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_4->addWidget(label_3, 3, 0, 1, 1);

        m_lineYamlPath = new QLineEdit(layoutWidget);
        m_lineYamlPath->setObjectName("m_lineYamlPath");

        gridLayout_4->addWidget(m_lineYamlPath, 3, 1, 1, 2);

        m_btnYamlOpen = new QPushButton(layoutWidget);
        m_btnYamlOpen->setObjectName("m_btnYamlOpen");

        gridLayout_4->addWidget(m_btnYamlOpen, 3, 3, 1, 1);

        m_comVersion = new QComboBox(layoutWidget);
        m_comVersion->addItem(QString());
        m_comVersion->addItem(QString());
        m_comVersion->addItem(QString());
        m_comVersion->setObjectName("m_comVersion");

        gridLayout_4->addWidget(m_comVersion, 0, 2, 1, 2);


        verticalLayout_3->addLayout(gridLayout_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");
        label_4->setFrameShadow(QFrame::Shadow::Sunken);
        label_4->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_2->addWidget(label_4);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName("gridLayout_3");
        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");
        label_5->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_3->addWidget(label_5, 0, 0, 1, 1);

        m_spinEpochs = new QSpinBox(layoutWidget);
        m_spinEpochs->setObjectName("m_spinEpochs");
        m_spinEpochs->setMaximum(1000000);
        m_spinEpochs->setValue(250);

        gridLayout_3->addWidget(m_spinEpochs, 0, 1, 1, 1);

        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");
        label_6->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_3->addWidget(label_6, 1, 0, 1, 1);

        m_spinBatchSize = new QSpinBox(layoutWidget);
        m_spinBatchSize->setObjectName("m_spinBatchSize");
        m_spinBatchSize->setMaximum(1000000);
        m_spinBatchSize->setValue(8);

        gridLayout_3->addWidget(m_spinBatchSize, 1, 1, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");
        label_7->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_3->addWidget(label_7, 2, 0, 1, 1);

        m_spinImageSize = new QSpinBox(layoutWidget);
        m_spinImageSize->setObjectName("m_spinImageSize");
        m_spinImageSize->setMaximum(1000000);
        m_spinImageSize->setValue(320);

        gridLayout_3->addWidget(m_spinImageSize, 2, 1, 1, 1);

        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName("label_8");
        label_8->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_3->addWidget(label_8, 3, 0, 1, 1);

        m_comboDevice = new QComboBox(layoutWidget);
        m_comboDevice->addItem(QString());
        m_comboDevice->addItem(QString());
        m_comboDevice->setObjectName("m_comboDevice");

        gridLayout_3->addWidget(m_comboDevice, 3, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout_3);


        verticalLayout_3->addLayout(verticalLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

        m_btnTrainPt = new QPushButton(layoutWidget);
        m_btnTrainPt->setObjectName("m_btnTrainPt");

        gridLayout->addWidget(m_btnTrainPt, 0, 1, 1, 1);

        m_btnStop = new QPushButton(layoutWidget);
        m_btnStop->setObjectName("m_btnStop");

        gridLayout->addWidget(m_btnStop, 0, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 3, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

        m_btn2Onnx = new QPushButton(layoutWidget);
        m_btn2Onnx->setObjectName("m_btn2Onnx");

        gridLayout->addWidget(m_btn2Onnx, 1, 1, 1, 1);

        m_btn2Kmodel = new QPushButton(layoutWidget);
        m_btn2Kmodel->setObjectName("m_btn2Kmodel");

        gridLayout->addWidget(m_btn2Kmodel, 1, 2, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 1, 3, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        splitter_2->addWidget(layoutWidget);
        splitter = new QSplitter(splitter_2);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName("layoutWidget1");
        verticalLayout = new QVBoxLayout(layoutWidget1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        m_btnDownload = new QPushButton(layoutWidget1);
        m_btnDownload->setObjectName("m_btnDownload");

        verticalLayout->addWidget(m_btnDownload);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        label_11 = new QLabel(layoutWidget1);
        label_11->setObjectName("label_11");
        label_11->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout->addWidget(label_11);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        label_12 = new QLabel(layoutWidget1);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 0, 0, 1, 2);

        m_linePtPath = new QLineEdit(layoutWidget1);
        m_linePtPath->setObjectName("m_linePtPath");

        gridLayout_2->addWidget(m_linePtPath, 0, 2, 1, 3);

        m_btnPtOpen = new QPushButton(layoutWidget1);
        m_btnPtOpen->setObjectName("m_btnPtOpen");

        gridLayout_2->addWidget(m_btnPtOpen, 0, 5, 1, 1);

        label_13 = new QLabel(layoutWidget1);
        label_13->setObjectName("label_13");

        gridLayout_2->addWidget(label_13, 1, 0, 1, 2);

        m_lineValPath = new QLineEdit(layoutWidget1);
        m_lineValPath->setObjectName("m_lineValPath");

        gridLayout_2->addWidget(m_lineValPath, 1, 2, 1, 3);

        m_btnValOpen = new QPushButton(layoutWidget1);
        m_btnValOpen->setObjectName("m_btnValOpen");

        gridLayout_2->addWidget(m_btnValOpen, 1, 5, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_5, 2, 0, 1, 1);

        m_btnDir2Onnx = new QPushButton(layoutWidget1);
        m_btnDir2Onnx->setObjectName("m_btnDir2Onnx");

        gridLayout_2->addWidget(m_btnDir2Onnx, 2, 1, 1, 2);

        m_Dir2Kmodel = new QPushButton(layoutWidget1);
        m_Dir2Kmodel->setObjectName("m_Dir2Kmodel");

        gridLayout_2->addWidget(m_Dir2Kmodel, 2, 3, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_6, 2, 4, 1, 2);


        verticalLayout->addLayout(gridLayout_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        splitter->addWidget(layoutWidget1);
        layoutWidget2 = new QWidget(splitter);
        layoutWidget2->setObjectName("layoutWidget2");
        verticalLayout_4 = new QVBoxLayout(layoutWidget2);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_10 = new QLabel(layoutWidget2);
        label_10->setObjectName("label_10");
        label_10->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_4->addWidget(label_10);

        m_editLog = new QTextEdit(layoutWidget2);
        m_editLog->setObjectName("m_editLog");
        m_editLog->setReadOnly(true);

        verticalLayout_4->addWidget(m_editLog);

        splitter->addWidget(layoutWidget2);
        splitter_2->addWidget(splitter);

        horizontalLayout_5->addWidget(splitter_2);

        yoloControl->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(yoloControl);
        statusbar->setObjectName("statusbar");
        yoloControl->setStatusBar(statusbar);

        retranslateUi(yoloControl);

        QMetaObject::connectSlotsByName(yoloControl);
    } // setupUi

    void retranslateUi(QMainWindow *yoloControl)
    {
        yoloControl->setWindowTitle(QCoreApplication::translate("yoloControl", "\347\256\200\346\230\223\350\256\255\347\273\203\345\267\245\345\205\267", nullptr));
        label->setText(QCoreApplication::translate("yoloControl", "\350\257\267\351\200\211\346\213\251\346\202\250\347\232\204YOLO\347\211\210\346\234\254: ", nullptr));
        label_9->setText(QCoreApplication::translate("yoloControl", "\344\275\277\347\224\250\347\232\204conda\347\216\257\345\242\203\345\220\215\347\247\260\346\230\257:", nullptr));
        m_comEvnName->setItemText(0, QCoreApplication::translate("yoloControl", "yolo_to_kmodel", nullptr));

        label_2->setText(QCoreApplication::translate("yoloControl", "\346\211\223\345\274\200YOLO\346\240\271\347\233\256\345\275\225:", nullptr));
        m_btnYoloOpen->setText(QCoreApplication::translate("yoloControl", "\346\211\223\345\274\200", nullptr));
        label_3->setText(QCoreApplication::translate("yoloControl", "\346\211\223\345\274\200\351\205\215\347\275\256\346\226\207\344\273\266:", nullptr));
        m_btnYamlOpen->setText(QCoreApplication::translate("yoloControl", "\346\211\223\345\274\200", nullptr));
        m_comVersion->setItemText(0, QCoreApplication::translate("yoloControl", "YOLOv11", nullptr));
        m_comVersion->setItemText(1, QCoreApplication::translate("yoloControl", "YOLOv8", nullptr));
        m_comVersion->setItemText(2, QCoreApplication::translate("yoloControl", "YOLOv5", nullptr));

        label_4->setText(QCoreApplication::translate("yoloControl", "\351\205\215\347\275\256\350\256\255\347\273\203\345\217\202\346\225\260:", nullptr));
        label_5->setText(QCoreApplication::translate("yoloControl", "Epochs:", nullptr));
        label_6->setText(QCoreApplication::translate("yoloControl", "Batch Size:", nullptr));
        label_7->setText(QCoreApplication::translate("yoloControl", "Image Size:", nullptr));
        label_8->setText(QCoreApplication::translate("yoloControl", "Devic:", nullptr));
        m_comboDevice->setItemText(0, QCoreApplication::translate("yoloControl", "GPU", nullptr));
        m_comboDevice->setItemText(1, QCoreApplication::translate("yoloControl", "CPU", nullptr));

        m_btnTrainPt->setText(QCoreApplication::translate("yoloControl", "\345\274\200\345\247\213\350\256\255\347\273\203", nullptr));
        m_btnStop->setText(QCoreApplication::translate("yoloControl", "\345\201\234\346\255\242\350\256\255\347\273\203", nullptr));
        m_btn2Onnx->setText(QCoreApplication::translate("yoloControl", "toOnnx", nullptr));
        m_btn2Kmodel->setText(QCoreApplication::translate("yoloControl", "toKmodel", nullptr));
        m_btnDownload->setText(QCoreApplication::translate("yoloControl", "\344\270\213\350\275\275\346\226\207\346\241\243", nullptr));
        label_11->setText(QCoreApplication::translate("yoloControl", "\344\270\215\350\256\255\347\273\203\347\233\264\346\216\245\350\275\254\345\214\226\351\234\200\350\246\201\346\217\220\344\276\233\346\250\241\345\236\213\346\226\207\344\273\266\345\222\214val\346\225\260\346\215\256\351\233\206\351\273\230\350\256\244\346\230\257best.onnx/best.pt", nullptr));
        label_12->setText(QCoreApplication::translate("yoloControl", "\346\250\241\345\236\213\346\226\207\344\273\266\350\267\257\345\276\204:", nullptr));
        m_btnPtOpen->setText(QCoreApplication::translate("yoloControl", "\346\211\223\345\274\200", nullptr));
        label_13->setText(QCoreApplication::translate("yoloControl", "val\346\225\260\346\215\256\351\233\206\350\267\257\345\276\204:", nullptr));
        m_btnValOpen->setText(QCoreApplication::translate("yoloControl", "\346\211\223\345\274\200", nullptr));
        m_btnDir2Onnx->setText(QCoreApplication::translate("yoloControl", "toOnnx", nullptr));
        m_Dir2Kmodel->setText(QCoreApplication::translate("yoloControl", "toKmodel", nullptr));
        label_10->setText(QCoreApplication::translate("yoloControl", "\346\211\223\345\215\260\346\227\245\345\277\227\344\277\241\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class yoloControl: public Ui_yoloControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YOLOCONTROL_H
