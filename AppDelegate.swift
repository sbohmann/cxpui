import AppKit

class AppDelegate: NSObject, NSApplicationDelegate, NSWindowDelegate {
    var window: NSWindow!

    func applicationDidFinishLaunching(_ notification: Notification) {
        let screenRect = NSScreen.main?.frame ?? NSRect()

        window = NSWindow(
            contentRect: NSMakeRect(
                screenRect.midX - 200,
                screenRect.midY - 200,
                400,
                200
            ),
            styleMask: [.titled, .closable, .resizable],
            backing: .buffered,
            defer: false
        )
        window.title = "Hello World App"
        window.center()
        window.makeKeyAndOrderFront(nil)

        NSApplication.shared.activate(ignoringOtherApps: true)

        let label = NSTextField(labelWithString: "Hello, World!")
        label.alignment = .center
        label.font = NSFont.systemFont(ofSize: 24)
        label.translatesAutoresizingMaskIntoConstraints = false // Enable Auto Layout
        window.contentView?.addSubview(label)

        if let contentView = window.contentView {
            NSLayoutConstraint.activate([
                label.leadingAnchor.constraint(equalTo: contentView.leadingAnchor),
                label.trailingAnchor.constraint(equalTo: contentView.trailingAnchor),
                label.centerYAnchor.constraint(equalTo: contentView.centerYAnchor),
                label.heightAnchor.constraint(lessThanOrEqualTo: contentView.heightAnchor)
            ])
        }

        window.delegate = self
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}

var delegate: AppDelegate! = nil

@_cdecl("start")
func start() {
    let app = NSApplication.shared
    delegate = AppDelegate()
    app.delegate = delegate
    app.run()
}

@_cdecl("line")
func line(x1: Float, y1: Float, x2: Float, y2: Float) {
    //delegate.
}
