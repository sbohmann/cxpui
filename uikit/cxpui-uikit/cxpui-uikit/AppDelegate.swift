import AppKit

class AppDelegate: NSObject, NSApplicationDelegate, NSWindowDelegate {
    var window: NSWindow?
    var windowView: WindowView?

    func applicationDidFinishLaunching(_ notification: Notification) {
        NSApplication.shared.activate(ignoringOtherApps: true)
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}

class WindowView: NSView {
    var appDelegate: AppDelegate?
    var cxpuiWindow: UnsafeMutablePointer<Window>?
    
    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)

        guard let context = NSGraphicsContext.current?.cgContext else { return }
        
        let bounds = bounds
        
        NSColor.white.setFill()
        context.fill(bounds)
        
        if let win = self.cxpuiWindow {
            if win.pointee.mainView.pointee.type == CustomView {
                win.pointee.mainView.withMemoryRebound(to: CustomView.self, capacity: 1) {
                    customViewPointer in
                    customViewPointer.pointee.paint(
                        GraphicsContext(
                            width: bounds.size.width,
                            height: bounds.size.height,
                            native_context: Unmanaged.passRetained(context).toOpaque()))
                }
            }
        }
    }
}

@_cdecl("start")
func start() -> UnsafeMutablePointer<Window> {
    let app = NSApplication.shared
    let appDelegate = AppDelegate()
    app.delegate = appDelegate
    let window = createWindow(appDelegate: appDelegate)
    return window
}

@_cdecl("Application_run")
func run() {
    NSApplication.shared.run()
}

func createWindow(appDelegate: AppDelegate) -> UnsafeMutablePointer<Window> {
    let screenRect = NSScreen.main?.frame ?? NSRect()
    
    let window = NSWindow(
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
    window.title = "cxpui Appkit Window"
    window.center()
    window.delegate = appDelegate
    window.makeKeyAndOrderFront(nil)

    let windowView = WindowView(frame: window.contentView?.bounds ?? NSRect.zero)
    windowView.autoresizingMask = [.width, .height]
    window.contentView?.addSubview(windowView)
    var cxpuiWindow = Window()
    cxpuiWindow.mainView = nil
    cxpuiWindow.native_view = Unmanaged.passRetained(window).toOpaque()
    let cxpuiWindowPointer = UnsafeMutablePointer<Window>.allocate(capacity: 1)
    cxpuiWindowPointer.initialize(to: cxpuiWindow)
    windowView.cxpuiWindow = cxpuiWindowPointer
    return cxpuiWindowPointer
}

@_cdecl("Window_set_main_view")
func setMainView(window: UnsafeMutablePointer<Window>, view: UnsafeMutablePointer<View>) {
    window.pointee.mainView = view
}

@_cdecl("line")
func line(gc: GraphicsContext, x1: CGFloat, y1: CGFloat, x2: CGFloat, y2: CGFloat) {
    let nativeContext = Unmanaged<CGContext>.fromOpaque(gc.native_context).takeUnretainedValue()
    NSColor.blue.setStroke()
    nativeContext.setLineWidth(2)
    nativeContext.move(to: CGPoint(x: x1, y: y1))
    nativeContext.addLine(to: CGPoint(x: x2, y: y2))
    nativeContext.strokePath()
}

@_cdecl("rect")
func rect(gc: GraphicsContext, x: CGFloat, y: CGFloat, w: CGFloat, h: CGFloat) {
    let nativeContext = Unmanaged<CGContext>.fromOpaque(gc.native_context).takeUnretainedValue()
    NSColor.red.setFill()
    nativeContext.fill(NSRect(x: x, y: y, width: w, height: h))
}

@_silgen_name("draw")
func draw(context: GraphicsContext)
