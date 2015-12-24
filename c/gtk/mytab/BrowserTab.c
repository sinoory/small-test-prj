#include "cmakeconfig.h"
#include "BrowserTab.h"


struct _BrowserTab {
    GtkHBox hbox;
    GtkWidget *icon;
    GtkWidget *title;
    GtkWidget *close;

};

G_DEFINE_TYPE(BrowserTab, browser_tab, GTK_TYPE_HBOX)  //for browser_tab_get_type()



static void browser_tab_init(BrowserTab *browserTab)
{
    browserTab->title = gtk_button_new_with_label("button");// gtk_label_new ("lable");

    //gtk_container_add(GTK_CONTAINER(browserTab),browserTab->title );
    gtk_box_pack_start(GTK_BOX(browserTab),browserTab->title,FALSE, FALSE, 0);
    gtk_widget_show(browserTab->title);

    GtkWidget* lable=gtk_label_new ("lable");
    gtk_box_pack_start(GTK_BOX(browserTab),lable,FALSE, FALSE, 0);
    gtk_widget_show(lable);

//    g_signal_connect_swapped(browserTab->entry, "icon-release", G_CALLBACK(searchEntryClearIconReleasedCallback), browserTab);
//    g_signal_connect_after(browserTab->entry, "changed", G_CALLBACK(searchEntryChangedCallback), browserTab);
}

static void browserTabFinalize(GObject *gObject)
{
    BrowserTab *browserTab = BROWSER_TAB(gObject);

//    G_OBJECT_CLASS(browser_tab_parent_class)->finalize(gObject);
}

static void browser_tab_class_init(BrowserTabClass *klass)
{
    GObjectClass *gObjectClass = G_OBJECT_CLASS(klass);

    gObjectClass->finalize = browserTabFinalize;
}

GtkWidget *browser_tab_new()
{

    GtkWidget *browserTab = GTK_WIDGET(g_object_new(BROWSER_TYPE_TAB, NULL));
    /*
    BROWSER_SEARCH_BAR(browserTab)->webView = g_object_ref(webView);

    WebKitFindController *controller = webkit_web_view_get_find_controller(webView);
    g_signal_connect_swapped(controller, "failed-to-find-text", G_CALLBACK(findControllerFailedToFindTextCallback), browserTab);
    g_signal_connect_swapped(controller, "found-text", G_CALLBACK(findControllerFoundTextCallback), browserTab);
    */
    return browserTab;
}


