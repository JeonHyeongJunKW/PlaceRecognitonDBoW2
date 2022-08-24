from PyQt5.QtWidgets import *
from PyQt5 import uic
from PyQt5.QtGui import *
from PyQt5.QtCore import Qt
from PyQt5.QtCore import QTimer
from PyQt5 import QtCore
import sys
import os
import ctypes
import time
import glob
form_class = uic.loadUiType("./UI/main_window.ui")[0]

class WindowClass(QMainWindow, form_class):
    def __init__(self):
        super(WindowClass,self).__init__()
        self.setupUi(self)
        lib_path = "./dll/libhj_placeDBow2.so"
        c_module = ctypes.cdll.LoadLibrary(lib_path)
        self.MakeDB = c_module.MakeDB
        self.FindQuIdx = c_module.FindImageIdx
        self.FindQueryImageInDB = c_module.FindQueryImageInDB
        self.SaveDB = c_module.SaveDB
        self.LoadDB = c_module.LoadDB

        self.buttonOpenSeq.clicked.connect(self.getSeqPath)
        self.buttonMakeDB.clicked.connect(self.makeDB)
        self.buttonSaveDB.clicked.connect(self.saveDB)
        self.buttonSelectQuery.clicked.connect(self.SelectImage)
        self.buttonQueryImage.clicked.connect(self.QueryImageIdx)
        self.buttonLoadDB.clicked.connect(self.loadDB)
        
        # self.buttonMakeDB.setEnabled(True)
        # self.buttonSelectQuery.setEnabled(True)
        # self.buttonLoadDB.setEnabled(True)
        
        self.OnAllButton()
        self.seqName ="./"
        self.QueryName=""
        self.buttonSaveDB.setDisabled(True)
        self.buttonMakeDB.setDisabled(True)
        self.buttonQueryImage.setDisabled(True)

    def getSeqPath(self):
        fname=QFileDialog.getExistingDirectory(self, "Open DataBase","./", QFileDialog.ShowDirsOnly| QFileDialog.DontResolveSymlinks)
        if len(fname) !=0:
            self.seqName =fname
            self.labelSeqPath.setText(self.seqName)
            self.buttonMakeDB.setEnabled(True)
            
            
    def makeDB(self):
        self.OffAllButton()
        time.sleep(0.010)
        self.MakeDB(self.seqName)
        self.OnAllButton()
        self.buttonQueryImage.setDisabled(True)
    def saveDB(self):
        DBname,_ = QFileDialog.getSaveFileName(self, 'Save File')
        os.makedirs(DBname)
        self.SaveDB(DBname)
    
    def loadDB(self):
        fname=QFileDialog.getExistingDirectory(self, "Open DataBase","./", QFileDialog.ShowDirsOnly| QFileDialog.DontResolveSymlinks)
        if len(fname) !=0:
            self.LoadDB(fname)
            self.labelDBPath.setText(fname)

    def OffAllButton(self):
        self.buttonOpenSeq.setDisabled(True)
        self.buttonMakeDB.setDisabled(True)
        self.buttonSelectQuery.setDisabled(True)
        self.buttonLoadDB.setDisabled(True)
        self.buttonQueryImage.setDisabled(True)
        self.buttonSaveDB.setDisabled(True)
    def OnAllButton(self):
        self.buttonOpenSeq.setEnabled(True)
        self.buttonMakeDB.setEnabled(True)
        self.buttonSelectQuery.setEnabled(True)
        self.buttonLoadDB.setEnabled(True)
        self.buttonQueryImage.setEnabled(True)
        self.buttonSaveDB.setEnabled(True)

    def SelectImage(self):
        self.QueryName,_=QFileDialog.getOpenFileName(self,directory=self.seqName)
        self.labelQuName.setText(self.QueryName)
        qPixmapVar = QPixmap()
        qPixmapVar.load(self.QueryName)
        #이미지 전처리
        qPixmapVar=qPixmapVar.scaledToWidth(self.labelQueryImage.width())
        idx = self.FindQuIdx(self.QueryName)
        self.QueryIdx.setStyleSheet("background-color: #F01807")
        self.QueryIdx.setText(str(idx)+"번")
        self.buttonQueryImage.setEnabled(True)
        self.labelQueryImage.setPixmap(qPixmapVar)

    def QueryImageIdx(self):
        idx =self.FindQueryImageInDB(self.QueryName)
        image_names = sorted(glob.glob(self.seqName+"/*"))
        name = image_names[idx]
        idx = self.FindQuIdx(name)
        if idx != -1:
            qPixmapVar = QPixmap()
            qPixmapVar.load(name)
            #이미지 전처리
            qPixmapVar=qPixmapVar.scaledToWidth(self.labelDBImage.width())
            self.ReturnIdx.setStyleSheet("background-color: #F01807")
            self.ReturnIdx.setText(str(idx)+"번")
            self.labelDBImage.setPixmap(qPixmapVar)
        else:
            self.ReturnIdx.setStyleSheet("background-color: #F01807")
            self.ReturnIdx.setText("매칭되는 장소가 없습니다.")
    



if __name__ == "__main__":
    os.environ["QT_AUTO_SCREEN_SCALE_FACTOR"] ='1'
    app = QApplication(sys.argv)
    app.setAttribute(Qt.AA_EnableHighDpiScaling)
    myWindow = WindowClass()
    myWindow.show()
    sys.exit(app.exec_())