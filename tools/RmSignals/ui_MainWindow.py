# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'ui_MainWindow.ui'
##
## Created by: Qt User Interface Compiler version 6.11.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QCheckBox, QComboBox, QFormLayout,
    QGroupBox, QHBoxLayout, QLabel, QMainWindow,
    QMenu, QMenuBar, QPushButton, QSizePolicy,
    QSpinBox, QVBoxLayout, QWidget)

from pyqtgraph import PlotWidget

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1188, 778)
        MainWindow.setMinimumSize(QSize(0, 0))
        MainWindow.setMaximumSize(QSize(16777215, 16777215))
        self.actionQuit = QAction(MainWindow)
        self.actionQuit.setObjectName(u"actionQuit")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.centralwidget.setEnabled(True)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.centralwidget.sizePolicy().hasHeightForWidth())
        self.centralwidget.setSizePolicy(sizePolicy)
        self.horizontalLayout_2 = QHBoxLayout(self.centralwidget)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.plotWidget = PlotWidget(self.centralwidget)
        self.plotWidget.setObjectName(u"plotWidget")

        self.horizontalLayout.addWidget(self.plotWidget)

        self.groupBox = QGroupBox(self.centralwidget)
        self.groupBox.setObjectName(u"groupBox")
        sizePolicy.setHeightForWidth(self.groupBox.sizePolicy().hasHeightForWidth())
        self.groupBox.setSizePolicy(sizePolicy)
        self.horizontalLayout_3 = QHBoxLayout(self.groupBox)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.formLayout = QFormLayout()
        self.formLayout.setObjectName(u"formLayout")
        self.labelGraph = QLabel(self.groupBox)
        self.labelGraph.setObjectName(u"labelGraph")

        self.formLayout.setWidget(0, QFormLayout.ItemRole.LabelRole, self.labelGraph)

        self.comboBoxGraph = QComboBox(self.groupBox)
        self.comboBoxGraph.setObjectName(u"comboBoxGraph")

        self.formLayout.setWidget(0, QFormLayout.ItemRole.FieldRole, self.comboBoxGraph)

        self.labelDoman = QLabel(self.groupBox)
        self.labelDoman.setObjectName(u"labelDoman")

        self.formLayout.setWidget(1, QFormLayout.ItemRole.LabelRole, self.labelDoman)

        self.comboBoxDomain = QComboBox(self.groupBox)
        self.comboBoxDomain.setObjectName(u"comboBoxDomain")

        self.formLayout.setWidget(1, QFormLayout.ItemRole.FieldRole, self.comboBoxDomain)

        self.checkBoxGrid = QCheckBox(self.groupBox)
        self.checkBoxGrid.setObjectName(u"checkBoxGrid")

        self.formLayout.setWidget(2, QFormLayout.ItemRole.LabelRole, self.checkBoxGrid)

        self.labelPeriod = QLabel(self.groupBox)
        self.labelPeriod.setObjectName(u"labelPeriod")

        self.formLayout.setWidget(3, QFormLayout.ItemRole.LabelRole, self.labelPeriod)

        self.spinBoxPeriod = QSpinBox(self.groupBox)
        self.spinBoxPeriod.setObjectName(u"spinBoxPeriod")
        self.spinBoxPeriod.setMinimum(10)
        self.spinBoxPeriod.setMaximum(1000)
        self.spinBoxPeriod.setValue(100)

        self.formLayout.setWidget(3, QFormLayout.ItemRole.FieldRole, self.spinBoxPeriod)

        self.labelNumPoints = QLabel(self.groupBox)
        self.labelNumPoints.setObjectName(u"labelNumPoints")

        self.formLayout.setWidget(4, QFormLayout.ItemRole.LabelRole, self.labelNumPoints)

        self.comboBoxNumPoints = QComboBox(self.groupBox)
        self.comboBoxNumPoints.setObjectName(u"comboBoxNumPoints")

        self.formLayout.setWidget(4, QFormLayout.ItemRole.FieldRole, self.comboBoxNumPoints)

        self.labelWindow = QLabel(self.groupBox)
        self.labelWindow.setObjectName(u"labelWindow")

        self.formLayout.setWidget(5, QFormLayout.ItemRole.LabelRole, self.labelWindow)

        self.comboBoxWindow = QComboBox(self.groupBox)
        self.comboBoxWindow.setObjectName(u"comboBoxWindow")

        self.formLayout.setWidget(5, QFormLayout.ItemRole.FieldRole, self.comboBoxWindow)

        self.labelBeta = QLabel(self.groupBox)
        self.labelBeta.setObjectName(u"labelBeta")

        self.formLayout.setWidget(6, QFormLayout.ItemRole.LabelRole, self.labelBeta)

        self.spinBoxAtten = QSpinBox(self.groupBox)
        self.spinBoxAtten.setObjectName(u"spinBoxAtten")
        self.spinBoxAtten.setValue(60)

        self.formLayout.setWidget(6, QFormLayout.ItemRole.FieldRole, self.spinBoxAtten)


        self.verticalLayout.addLayout(self.formLayout)

        self.pushButtonPlayPause = QPushButton(self.groupBox)
        self.pushButtonPlayPause.setObjectName(u"pushButtonPlayPause")

        self.verticalLayout.addWidget(self.pushButtonPlayPause)


        self.horizontalLayout_3.addLayout(self.verticalLayout)


        self.horizontalLayout.addWidget(self.groupBox)


        self.horizontalLayout_2.addLayout(self.horizontalLayout)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 1188, 21))
        self.menuFile = QMenu(self.menubar)
        self.menuFile.setObjectName(u"menuFile")
        MainWindow.setMenuBar(self.menubar)

        self.menubar.addAction(self.menuFile.menuAction())
        self.menuFile.addAction(self.actionQuit)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"RadioMon Signals", None))
        self.actionQuit.setText(QCoreApplication.translate("MainWindow", u"Quit", None))
        self.groupBox.setTitle(QCoreApplication.translate("MainWindow", u"Settings", None))
        self.labelGraph.setText(QCoreApplication.translate("MainWindow", u"Graph", None))
        self.labelDoman.setText(QCoreApplication.translate("MainWindow", u"Domain", None))
        self.checkBoxGrid.setText(QCoreApplication.translate("MainWindow", u"Enable Gird", None))
        self.labelPeriod.setText(QCoreApplication.translate("MainWindow", u"Sample Size(ms)", None))
#if QT_CONFIG(tooltip)
        self.spinBoxPeriod.setToolTip("")
#endif // QT_CONFIG(tooltip)
        self.labelNumPoints.setText(QCoreApplication.translate("MainWindow", u"Number of Points", None))
        self.labelWindow.setText(QCoreApplication.translate("MainWindow", u"Window", None))
        self.labelBeta.setText(QCoreApplication.translate("MainWindow", u"Attenuation (dB)", None))
        self.pushButtonPlayPause.setText(QCoreApplication.translate("MainWindow", u"PushButton", None))
        self.menuFile.setTitle(QCoreApplication.translate("MainWindow", u"File", None))
    # retranslateUi

