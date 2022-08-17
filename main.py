from PyQt5.QtWidgets import *
from PyQt5 import uic
from PyQt5.QtGui import *
from PyQt5.QtCore import Qt
import sys
import os
import ctypes
form_class = uic.loadUiType("./UI/main_window.ui")[0]

class WindowClass(QMainWindow, form_class):
    def __init__(self):
        super(WindowClass,self).__init__()
        self.setupUi(self)
        lib_path = "./dll/libhj_placeDBow2.so"
        c_module = ctypes.cdll.LoadLibrary(lib_path)
        self.test = c_module.hi
        self.buttonOpenSeq.clicked.connect(self.getSeqPath)
        self.buttonMakeDB.clicked.connect(self.makeDB)
        self.seqName =""
    def getSeqPath(self):
        fname=QFileDialog.getExistingDirectory(self, "Open DataBase","./", QFileDialog.ShowDirsOnly| QFileDialog.DontResolveSymlinks)
        if len(fname) !=0:
            self.seqName =fname
            self.labelSeqPath.setText(self.seqName)
            self.buttonMakeDB.setEnabled(True)
    def makeDB(self):
        self.test()

if __name__ == "__main__":
    
    
    os.environ["QT_AUTO_SCREEN_SCALE_FACTOR"] ='1'
    app = QApplication(sys.argv)
    app.setAttribute(Qt.AA_EnableHighDpiScaling)
    myWindow = WindowClass()
    myWindow.show()
    sys.exit(app.exec_())