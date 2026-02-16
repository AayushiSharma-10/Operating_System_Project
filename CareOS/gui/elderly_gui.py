#!/usr/bin/env python3
import gi, os, pty, threading, subprocess, shutil
gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, GLib, Pango, Gdk

class ElderlyHome(Gtk.Window):
    def __init__(self):
        super().__init__(title="MiniOS – Elderly Mode")
        self.set_default_size(700, 500)

        # === CENTER WINDOW ===
        self.set_position(Gtk.WindowPosition.CENTER)
        self.set_resizable(True)

        self.apply_css()

        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=25)
        vbox.set_border_width(30)
        self.add(vbox)

        title = Gtk.Label(label="MiniOS – Elderly Home")
        title.set_name("title-label")
        vbox.pack_start(title, False, False, 10)

        grid = Gtk.Grid(column_spacing=25, row_spacing=25)
        vbox.pack_start(grid, True, False, 10)

        self.add_big_button(grid, 0, 0, "Open Shell", self.open_shell)
        self.add_big_button(grid, 1, 0, "Notes", self.open_notes)
        self.add_big_button(grid, 0, 1, "Calculator", self.open_calc)
        self.add_big_button(grid, 1, 1, "Exit", Gtk.main_quit)

    def apply_css(self):
        css = b"""
        window { background: #1E272E; }
        #title-label { color: white; font-size: 32px; font-weight: bold; }
        button {
            font-size: 20px; padding: 25px;
            background: #3498DB; color: white;
            border-radius: 12px;
        }
        button:hover { background: #2574A9; }
        """
        provider = Gtk.CssProvider()
        provider.load_from_data(css)
        Gtk.StyleContext.add_provider_for_screen(
            Gdk.Screen.get_default(), provider,
            Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
        )

    def add_big_button(self, grid, col, row, text, cb):
        btn = Gtk.Button(label=text)
        btn.set_size_request(240, 90)
        btn.connect("clicked", cb)
        grid.attach(btn, col, row, 1, 1)

    def open_shell(self, widget):
        win = ShellWindow()
        win.show_all()

    def open_notes(self, widget):
        editor = shutil.which("gedit") or shutil.which("xed") or shutil.which("mousepad")
        if editor:
            subprocess.Popen([editor])

    def open_calc(self, widget):
        calc = shutil.which("gnome-calculator") or shutil.which("galculator")
        if calc:
            subprocess.Popen([calc])


class ShellWindow(Gtk.Window):
    def __init__(self):
        super().__init__(title="MiniOS Shell")
        self.set_default_size(900, 600)

        # === CENTER WINDOW ===
        self.set_position(Gtk.WindowPosition.CENTER)
        self.set_resizable(True)

        vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
        vbox.set_border_width(15)
        self.add(vbox)

        # === SCROLLABLE SHELL OUTPUT ===
        scroll = Gtk.ScrolledWindow()
        scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        vbox.pack_start(scroll, True, True, 10)

        self.textview = Gtk.TextView()
        self.textview.set_editable(False)
        self.textview.set_cursor_visible(False)
        self.textview.modify_font(Pango.FontDescription("Monospace 18"))
        scroll.add(self.textview)

        # Input area
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=12)
        vbox.pack_start(hbox, False, False, 5)

        self.entry = Gtk.Entry()
        self.entry.modify_font(Pango.FontDescription("Monospace 18"))
        self.entry.connect("activate", self.run_command)
        hbox.pack_start(self.entry, True, True, 5)

        run_btn = Gtk.Button(label="Run")
        run_btn.connect("clicked", self.run_command)
        hbox.pack_start(run_btn, False, False, 5)

        # Create PTY
        self.master, self.slave = pty.openpty()

        # Launch MiniOS shell
        self.proc = subprocess.Popen(
            ["./shell"],
            stdin=self.slave,
            stdout=self.slave,
            stderr=self.slave,
            universal_newlines=True,
            bufsize=0
        )

        threading.Thread(target=self.read_output, daemon=True).start()

    def read_output(self):
        while True:
            try:
                data = os.read(self.master, 1024).decode()
                if not data:
                    break
                GLib.idle_add(self.append_text, data)
            except:
                break

    def append_text(self, text):
        buf = self.textview.get_buffer()
        buf.insert(buf.get_end_iter(), text)

    def run_command(self, widget):
        cmd = self.entry.get_text() + "\n"
        self.entry.set_text("")
        os.write(self.master, cmd.encode())


if __name__ == "__main__":
    win = ElderlyHome()
    win.connect("destroy", Gtk.main_quit)
    win.show_all()
    Gtk.main()
