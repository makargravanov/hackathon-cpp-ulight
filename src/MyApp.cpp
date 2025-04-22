#include "MyApp.h"
#include <iostream>
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

MyApp::MyApp() {
	///
	/// Create our main App instance.
	///
	app_ = App::Create();

	///
	/// Create a resizable window by passing by OR'ing our window flags with
	/// kWindowFlags_Resizable.
	///
	window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
							 false, kWindowFlags_Titled | kWindowFlags_Resizable);

	///
	/// Create our HTML overlay-- we don't care about its initial size and
	/// position because it'll be calculated when we call OnResize() below.
	///
	overlay_ = Overlay::Create(window_, 1, 1, 0, 0);

	///
	/// Force a call to OnResize to perform size/layout of our overlay.
	///
	OnResize(window_.get(), window_->width(), window_->height());

	///
	/// Load a page into our overlay's View
	///
	overlay_->view()->LoadURL("file:///app.html");

	///
	/// Register our MyApp instance as an AppListener so we can handle the
	/// App's OnUpdate event below.
	///
	app_->set_listener(this);

	///
	/// Register our MyApp instance as a WindowListener so we can handle the
	/// Window's OnResize event below.
	///
	window_->set_listener(this);

	///
	/// Register our MyApp instance as a LoadListener so we can handle the
	/// View's OnFinishLoading and OnDOMReady events below.
	///
	overlay_->view()->set_load_listener(this);

	///
	/// Register our MyApp instance as a ViewListener so we can handle the
	/// View's OnChangeCursor and OnChangeTitle events below.
	///
	overlay_->view()->set_view_listener(this);
}

MyApp::~MyApp() {
}

void MyApp::Run() {
	app_->Run();
}

void MyApp::OnUpdate() {
	///
	/// This is called repeatedly from the application's update loop.
	///
	/// You should update any app logic here.
	///
}

void MyApp::OnClose(ultralight::Window* window) {
	app_->Quit();
}

void MyApp::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
	///
	/// This is called whenever the window changes size (values in pixels).
	///
	/// We resize our overlay here to take up the entire window.
	///
	overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View* caller,
							uint64_t frame_id,
							bool is_main_frame,
							const String& url) {
	///
	/// This is called when a frame finishes loading on the page.
	///
}

JSValueRef createFile(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
					  size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	if (argumentCount > 0) {
        // Преобразуем первый аргумент в строку
        JSStringRef jsName = JSValueToStringCopy(ctx, arguments[0], nullptr);
        size_t bufferSize = JSStringGetMaximumUTF8CStringSize(jsName);
        char* buffer = new char[bufferSize];
        JSStringGetUTF8CString(jsName, buffer, bufferSize);
        
        // Формируем имя файла, добавляя расширение ".aurora" 
        std::string fileName = std::string(buffer) + ".aurora";
        
        // Создаем файл в директории исполняемого файла
        std::ofstream file(fs::current_path() / fileName);
        file.close();
        
        // Освобождаем выделенную память и ресурсы
        delete[] buffer;
        JSStringRelease(jsName);
    }
    return JSValueMakeUndefined(ctx);
}
JSValueRef checkFileExists(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
						   size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
	bool exists = false;
	if (argumentCount > 0) {
		// Преобразуем первый аргумент в строку
		JSStringRef jsName = JSValueToStringCopy(ctx, arguments[0], nullptr);
		size_t bufferSize = JSStringGetMaximumUTF8CStringSize(jsName);
		char* buffer = new char[bufferSize];
		JSStringGetUTF8CString(jsName, buffer, bufferSize);

		// Проверяем существование файла
		exists = fs::exists(buffer);

		delete[] buffer;
		JSStringRelease(jsName);
	}
	return JSValueMakeBoolean(ctx, exists);
}

// Use LoadListener::OnDOMReady to wait for the DOM to load.
void MyApp::OnDOMReady(View* caller,
					   uint64_t frame_id,
					   bool is_main_frame,
					   const String& url) {
	// Ignore DOMReady events from child frames.
	if (!is_main_frame)
		return;

	// Acquire the JS execution context for the current page.
	auto scoped_context = caller->LockJSContext();

	// Typecast to the underlying JSContextRef.
	JSContextRef ctx = (*scoped_context);

	JSStringRef name = JSStringCreateWithUTF8CString("checkFileExists");
	JSObjectRef func = JSObjectMakeFunctionWithCallback(ctx, name, checkFileExists);
	JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
	JSObjectSetProperty(ctx, globalObj, name, func, 0, 0);
	JSStringRelease(name);

	JSStringRef name1 = JSStringCreateWithUTF8CString("createFile");
	JSObjectRef func1 = JSObjectMakeFunctionWithCallback(ctx, name, createFile);
	JSObjectSetProperty(ctx, globalObj, name1, func1, 0, 0);
	JSStringRelease(name1);
}

void MyApp::OnChangeCursor(ultralight::View* caller,
						   Cursor cursor) {
	///
	/// This is called whenever the page requests to change the cursor.
	///
	/// We update the main window's cursor here.
	///
	window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View* caller,
						  const String& title) {
	///
	/// This is called whenever the page requests to change the title.
	///
	/// We update the main window's title here.
	///
	window_->SetTitle(title.utf8().data());
}
