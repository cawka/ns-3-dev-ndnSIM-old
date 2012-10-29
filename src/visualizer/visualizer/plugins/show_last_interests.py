import gobject
import gtk

import ns.core
import ns.network
import ns.visualizer

from visualizer.base import InformationWindow
from visualizer.higcontainer import HIGContainer
from kiwi.ui.objectlist import ObjectList, Column

class ShowLastInterests(InformationWindow):
    class PacketList(gtk.ScrolledWindow):
        (
            COLUMN_TIME,
            COLUMN_INTERFACE,
            COLUMN_CONTENTS,
            ) = range(3)

        def __init__(self):
            super(ShowLastInterests.PacketList, self).__init__()
            self.set_properties(hscrollbar_policy=gtk.POLICY_AUTOMATIC,
                                vscrollbar_policy=gtk.POLICY_AUTOMATIC)
            self.table_model = gtk.ListStore(*([str]*3))
            treeview = gtk.TreeView(self.table_model)
            treeview.show()
            self.add(treeview)

            def add_column(descr, colid):
                column = gtk.TreeViewColumn(descr, gtk.CellRendererText(), text=colid)
                treeview.append_column(column)

            add_column("Time", self.COLUMN_TIME)
            add_column("Interface", self.COLUMN_INTERFACE)
            add_column("Contents", self.COLUMN_CONTENTS)

        def update(self, node, packet_list):
            self.table_model.clear()
            for sample in packet_list:
                if sample.device is None:
                    interface_name = "(unknown)"
                else:
                    interface_name = ns.core.Names.FindName(sample.device)
                    if not interface_name:
                        interface_name = "%i" % sample.device.GetIfIndex()

                newpkt = sample.packet.Copy ()

                pppHeader = ns.point_to_point.PppHeader()
                newpkt.RemoveHeader (pppHeader)
                interest = ns.ndnSIM.ndn.InterestHeader.GetInterest (newpkt)

                if (str(interest.GetName ()).split('/')[1] == "limit"):
                    tree_iter = self.table_model.append()
                    self.table_model.set(tree_iter,
                                         self.COLUMN_TIME, str(sample.time.GetSeconds()),
                                         self.COLUMN_INTERFACE, interface_name,
                                         self.COLUMN_CONTENTS, str(interest.GetName ())
                                         )


    def __init__(self, visualizer, node_index):
        InformationWindow.__init__(self)
        self.win = gtk.Dialog(parent=visualizer.window,
                              flags=gtk.DIALOG_DESTROY_WITH_PARENT|gtk.DIALOG_NO_SEPARATOR,
                              buttons=(gtk.STOCK_CLOSE, gtk.RESPONSE_CLOSE))
        self.win.connect("response", self._response_cb)
        self.win.set_title("Last packets for node %i" % node_index) 
        self.visualizer = visualizer
        self.viz_node = visualizer.get_node(node_index)
        self.node = ns.network.NodeList.GetNode(node_index)

        def smart_expand(expander, vbox):
            vbox.set_child_packing(expander, expand=True, fill=True, padding=0, pack_type=gtk.PACK_START)

        self.tx_list = self.PacketList()
        self.tx_list.show()
        self.win.vbox.add (self.tx_list)

        def update_capture_options():
            self.visualizer.simulation.lock.acquire()
            try:
                self.packet_capture_options = ns.visualizer.PyViz.PacketCaptureOptions ()
                self.packet_capture_options.mode = ns.visualizer.PyViz.PACKET_CAPTURE_FILTER_HEADERS_AND
                self.packet_capture_options.headers = [ns.ndnSIM.ndn.InterestHeader.GetTypeId ()]
                self.packet_capture_options.numLastPackets = 100

                self.visualizer.simulation.sim_helper.SetPacketCaptureOptions(
                    self.node.GetId(), self.packet_capture_options)
            finally:
                self.visualizer.simulation.lock.release()
        
        # - options
        update_capture_options ()

        self.visualizer.add_information_window(self)
        self.win.set_default_size(600, 300)
        self.win.show()

    def _response_cb(self, win, response):
        self.win.destroy()
        self.visualizer.remove_information_window(self)
    
    def update(self):
        last_packets = self.visualizer.simulation.sim_helper.GetLastPackets(self.node.GetId())
        self.tx_list.update(self.node, last_packets.lastTransmittedPackets)


def populate_node_menu(viz, node, menu):
    menu_item = gtk.MenuItem("Show Last Interests")
    menu_item.show()

    def _show_it(dummy_menu_item):
        ShowLastInterests(viz, node.node_index)

    menu_item.connect("activate", _show_it)
    menu.add(menu_item)

def register(viz):
    viz.connect("populate-node-menu", populate_node_menu)
