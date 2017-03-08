#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>

#if __IPHONE_8_0 && !TARGET_IPHONE_SIMULATOR  // check if sdk/target supports metal
#   import <Metal/Metal.h>
#   import <QuartzCore/CAMetalLayer.h>
//#   define HAS_METAL_SDK
#endif

#include <bgfx/bgfxplatform.h>
#ifdef HAS_METAL_SDK
static	id<MTLDevice>  m_device = NULL;
#else
static	void* m_device = NULL;
#endif

static noob::application* app;

@interface ViewController : UIViewController
@end
@implementation ViewController
- (BOOL)prefersStatusBarHidden
{
	return YES;
}
@end


@interface View : UIView
{
	CADisplayLink* m_displayLink;
}

@end

@implementation View

+ (Class)layerClass
{
#ifdef HAS_METAL_SDK
	Class metalClass = NSClassFromString(@"CAMetalLayer");    // is metal runtime sdk available
	if ( metalClass != nil)
	{
		m_device = MTLCreateSystemDefaultDevice(); // is metal supported on this device (is there a better way to do this - without creating device ?)
		if (m_device)
		{
			[m_device retain];
			return metalClass;
		}
	}
#endif

	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)rect
{
	self = [super initWithFrame:rect];

	if (nil == self)
	{
		return nil;
	}

//	bgfx::PlatformData pd;
//	pd.ndt          = NULL;
//	pd.nwh          = self.layer;
//	pd.context      = m_device;
//	pd.backBuffer   = NULL;
//	pd.backBufferDS = NULL;
//	bgfx::setPlatformData(pd);

	return self;
}

- (void)layoutSubviews
{
	uint32_t frameW = (uint32_t)(self.contentScaleFactor * self.frame.size.width);
	uint32_t frameH = (uint32_t)(self.contentScaleFactor * self.frame.size.height);
//	s_ctx->m_eventQueue.postSizeEvent(s_defaultWindow, frameW, frameH);
}

- (void)start
{
	if (nil == m_displayLink)
	{
		m_displayLink = [self.window.screen displayLinkWithTarget:self selector:@selector(renderFrame)];
		//[m_displayLink setFrameInterval:1];
		//[m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		//		[m_displayLink addToRunLoop:[NSRunLoop currentRunLoop]];
		[m_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
	}
}

- (void)stop
{
	if (nil != m_displayLink)
	{
		[m_displayLink invalidate];
		m_displayLink = nil;
	}
}

- (void)renderFrame
{
//	bgfx::renderFrame();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch = [[event allTouches] anyObject];
	CGPoint touchLocation = [touch locationInView:self];

//	s_ctx->m_eventQueue.postMouseEvent(s_defaultWindow, touchLocation.x, touchLocation.y, 0);
//	s_ctx->m_eventQueue.postMouseEvent(s_defaultWindow, touchLocation.x, touchLocation.y, 0, MouseButton::Left, true);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch = [[event allTouches] anyObject];
	CGPoint touchLocation = [touch locationInView:self];
//	s_ctx->m_eventQueue.postMouseEvent(s_defaultWindow, touchLocation.x, touchLocation.y, 0, MouseButton::Left, false);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch = [[event allTouches] anyObject];
	CGPoint touchLocation = [touch locationInView:self];
//	s_ctx->m_eventQueue.postMouseEvent(s_defaultWindow, touchLocation.x, touchLocation.y, 0);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *touch = [[event allTouches] anyObject];
	CGPoint touchLocation = [touch locationInView:self];
//	s_ctx->m_eventQueue.postMouseEvent(s_defaultWindow, touchLocation.x, touchLocation.y, 0, MouseButton::Left, false);
}

@end

@interface AppDelegate : UIResponder<UIApplicationDelegate>
{
	UIWindow* m_window;
	View* m_view;
}

@property (nonatomic, retain) UIWindow* m_window;
@property (nonatomic, retain) View* m_view;

@end

@implementation AppDelegate

@synthesize m_window;
@synthesize m_view;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{

	CGRect rect = [ [UIScreen mainScreen] bounds];
	m_window = [ [UIWindow alloc] initWithFrame: rect];
	m_view = [ [View alloc] initWithFrame: rect];

	[m_window addSubview: m_view];

	UIViewController *viewController = [[ViewController alloc] init];
	viewController.view = m_view;

	[m_window setRootViewController:viewController];
	[m_window makeKeyAndVisible];

	[m_window makeKeyAndVisible];

	// float scaleFactor = [[UIScreen mainScreen] scale]; // should use this, but ui is too small on ipad retina
	float scaleFactor = 1.0f;
	[m_view setContentScaleFactor: scaleFactor ];

	app = new noob::application();
//	s_ctx = new Context((uint32_t)(scaleFactor*rect.size.width), (uint32_t)(scaleFactor*rect.size.height));
	return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
//	s_ctx->m_eventQueue.postSuspendEvent(s_defaultWindow, Suspend::WillSuspend);
	[m_view stop];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
//	s_ctx->m_eventQueue.postSuspendEvent(s_defaultWindow, Suspend::DidSuspend);
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
//	s_ctx->m_eventQueue.postSuspendEvent(s_defaultWindow, Suspend::WillResume);
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
//	s_ctx->m_eventQueue.postSuspendEvent(s_defaultWindow, Suspend::DidResume);
	[m_view start];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	[m_view stop];
}

- (void)dealloc
{
	[m_window release];
	[m_view release];
	[super dealloc];
}

@end

int main(int _argc, char* _argv[])
{
	NSAutoreleasePool* pool = [ [NSAutoreleasePool alloc] init];
	int exitCode = UIApplicationMain(_argc, _argv, @"UIApplication", NSStringFromClass([AppDelegate class]) );
	[pool release];
	return exitCode;
}
