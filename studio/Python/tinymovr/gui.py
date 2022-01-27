
import sys

from PySide2 import QtCore
from PySide2.QtWidgets import QApplication, QMainWindow, QWidget, QFrame, QHBoxLayout, QVBoxLayout, QComboBox, QTreeWidget, QTreeWidgetItem
import pyqtgraph as pg

from tinymovr.constants import app_name

class MainWindow(QMainWindow):

    def __init__(self):
        super(MainWindow, self).__init__()

        self.setWindowTitle(app_name)
        self.make_dropdown()
        self.make_tree()
        self.make_graph()

        left_frame = QFrame(self)
        left_layout = QVBoxLayout()
        left_frame.setLayout(left_layout)
        left_layout.addWidget(self.dropdown_widget)
        left_layout.addWidget(self.tree_widget)

        main_layout = QHBoxLayout()
        main_layout.addWidget(left_frame)
        main_layout.addWidget(self.graph_widget)

        main_widget = QWidget()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)

    def make_dropdown(self):
        self.dropdown_widget = QComboBox()
        self.dropdown_widget.addItem("tm1")
        self.dropdown_widget.addItem("tm2")

    def make_tree(self):
        self.tree_widget = QTreeWidget()
        headers = ["Attributes"]
        self.tree_widget.setHeaderLabels(headers)
        tree_widget_item1 = QTreeWidgetItem(["item1"])
        tree_child_1 = QTreeWidgetItem(["item1_2"])
        tree_child_1.setCheckState(0, QtCore.Qt.Unchecked)
        tree_widget_item1.addChild(tree_child_1)
        tree_widget_item2 = QTreeWidgetItem(["item2"])
        tree_child_2 = QTreeWidgetItem(["item2_2"])
        tree_child_2.setCheckState(0, QtCore.Qt.Unchecked)
        tree_widget_item2.addChild(tree_child_2)
        self.tree_widget.addTopLevelItem(tree_widget_item1)
        self.tree_widget.addTopLevelItem(tree_widget_item2)

    def make_graph(self):
        self.graph_widget = pg.PlotWidget()

        hour = [1,2,3,4,5,6,7,8,9,10]
        temperature = [30,32,34,32,33,31,29,32,35,45]

        # plot data: x, y values
        self.graph_widget.plot(hour, temperature)

def spawn():
    app = QApplication(sys.argv)
    w = MainWindow()
    w.show()
    app.exec_()