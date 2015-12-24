#include <gtk/gtk.h>

/**
*callback function of button
*@param w widget
*@param userdata
*/
void hello_proc(GtkWidget* w,gpointer userdata)
{
	
}

#define WIDTH 480
#define HEIGHT 320

int main(int argc,char** argv)
{
    gtk_init(&argc,&argv);
	GtkWidget* main_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(main_win,WIDTH,HEIGHT);
	gtk_container_set_border_width(GTK_CONTAINER(main_win),5);
    
    //创建垂直布局
	GtkWidget* v = gtk_vbox_new(FALSE,5);
	gtk_container_add(GTK_CONTAINER(main_win),v);

    //创建按钮
	GtkWidget* b = gtk_button_new_with_label("Relief Normal");
	//设置按钮样式
	gtk_button_set_relief(GTK_BUTTON(b),GTK_RELIEF_NORMAL);
	//将按钮加入到布局中
	gtk_box_pack_start(GTK_BOX(v),b,FALSE,FALSE,0);
	//为按钮加入单击事件响应函数
	g_signal_connect(G_OBJECT(b),"clicked",G_CALLBACK(hello_proc),NULL);

    //创建按钮
	b = gtk_button_new_with_label("Relief Half");
	//设置按钮的样式
	gtk_button_set_relief(GTK_BUTTON(b),GTK_RELIEF_HALF);
	//将按钮加入到布局中
	gtk_box_pack_start(GTK_BOX(v),b,FALSE,FALSE,0);
	//添加事件处理函数
	g_signal_connect(G_OBJECT(b),"clicked",G_CALLBACK(hello_proc),NULL);

    //创建按钮
	b = gtk_button_new_with_label("Relief None");
	//设置按钮样式
	gtk_button_set_relief(GTK_BUTTON(b),GTK_RELIEF_NONE);
	//将按钮加入到布局中
	gtk_box_pack_start(GTK_BOX(v),b,FALSE,FALSE,0);
	//为按钮加入单击事件处理函数
	g_signal_connect(G_OBJECT(b),"clicked",G_CALLBACK(hello_proc),NULL);

	g_signal_connect_swapped(G_OBJECT(main_win),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	
	//显示主窗口控件及其所有子控件
	gtk_widget_show_all(main_win);
	gtk_main();

    return 0;
}
