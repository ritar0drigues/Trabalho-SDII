#include <gtk/gtk.h>
#include <httplib.h>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Widgets globais
GtkWidget *window;
GtkWidget *ip_entry;
GtkWidget *port_entry;
GtkWidget *file_chooser_button;
GtkWidget *process_button;
GtkWidget *result_label;
GtkWidget *letras_value;
GtkWidget *numeros_value;
GtkWidget *status_label;

std::string lerArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(arquivo)),
                       std::istreambuf_iterator<char>());
}

void on_process_clicked(GtkWidget *widget, gpointer data) {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));
    
    if (!filename) {
        gtk_label_set_text(GTK_LABEL(status_label), "❌ Por favor, selecione um arquivo");
        return;
    }

    gtk_label_set_text(GTK_LABEL(status_label), "🔄 Processando arquivo...");
    gtk_widget_set_sensitive(process_button, FALSE);

    // Atualizar interface
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }

    std::string conteudo = lerArquivo(filename);
    if (conteudo.empty()) {
        gtk_label_set_text(GTK_LABEL(status_label), "❌ Erro ao ler arquivo");
        gtk_widget_set_sensitive(process_button, TRUE);
        g_free(filename);
        return;
    }

    // Obter IP e porta digitados
    std::string ip = gtk_entry_get_text(GTK_ENTRY(ip_entry));
    std::string port = gtk_entry_get_text(GTK_ENTRY(port_entry));
    std::string url = ip + ":" + port;

    // Fazer requisição HTTP
    httplib::Client cli(url.c_str());
    auto res = cli.Post("/processar", conteudo, "text/plain");
    
    if (res && res->status == 200) {
        try {
            json resultado = json::parse(res->body);
            std::string letras = resultado["letras"];
            std::string numeros = resultado["numeros"];
            
            gtk_label_set_text(GTK_LABEL(letras_value), letras.c_str());
            gtk_label_set_text(GTK_LABEL(numeros_value), numeros.c_str());
            gtk_label_set_text(GTK_LABEL(status_label), "✅ Arquivo processado com sucesso!");
        } catch (const std::exception& e) {
            gtk_label_set_text(GTK_LABEL(status_label), "❌ Erro ao processar resposta");
        }
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "❌ Erro na conexão com o servidor");
    }
    
    gtk_widget_set_sensitive(process_button, TRUE);
    g_free(filename);
}

void apply_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    const gchar *css_data = R"(
        window { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); }
        .main-container { background: rgba(255,255,255,0.95); border-radius: 20px; padding: 30px; margin: 20px; box-shadow:0 20px 40px rgba(0,0,0,0.15);}
        .title { font-size: 28px; font-weight:bold; color:#2c3e50; margin-bottom:10px; }
        .subtitle { font-size:14px; color:#7f8c8d; margin-bottom:30px; }
        .file-section, .network-section { background:#f8f9fa; border-radius:12px; padding:20px; margin-bottom:25px; border:2px dashed #dee2e6; }
        .process-button { background: linear-gradient(45deg,#4facfe 0%,#00f2fe 100%); color:white; border:none; border-radius:25px; padding:12px 30px; font-size:16px; font-weight:bold; margin:15px 0; box-shadow:0 4px 15px rgba(79,172,254,0.4); transition:all 0.3s ease; }
        .process-button:hover { transform:translateY(-2px); box-shadow:0 6px 20px rgba(79,172,254,0.6);}
        .result-section { background: linear-gradient(135deg,#667eea 0%,#764ba2 100%); border-radius:15px; padding:25px; color:white; }
        .result-title { font-size:20px; font-weight:bold; margin-bottom:20px; text-align:center; }
        .result-card { background: rgba(255,255,255,0.2); border-radius:10px; padding:15px; margin:10px 0; backdrop-filter:blur(10px);}
        .result-label { font-size:14px; opacity:0.9; margin-bottom:5px; }
        .result-value { font-size:24px; font-weight:bold; color:#fff; }
        .status-label { font-size:14px; margin-top:15px; padding:10px; border-radius:8px; text-align:center; background: rgba(0,0,0,0.05);}
    )";
    gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    apply_css();

    // Janela principal
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Contador de Caracteres");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 650);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(main_box), "main-container");
    gtk_container_add(GTK_CONTAINER(window), main_box);

    GtkWidget *title = gtk_label_new("📊 Contador de Caracteres");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "title");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);

    GtkWidget *subtitle = gtk_label_new("Digite IP e porta do servidor e selecione um arquivo");
    gtk_style_context_add_class(gtk_widget_get_style_context(subtitle), "subtitle");
    gtk_box_pack_start(GTK_BOX(main_box), subtitle, FALSE, FALSE, 0);

    // Seção de IP e porta
    GtkWidget *network_section = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_style_context_add_class(gtk_widget_get_style_context(network_section), "network-section");
    gtk_box_pack_start(GTK_BOX(main_box), network_section, FALSE, FALSE, 0);

    GtkWidget *ip_label = gtk_label_new("🌐 IP do servidor:");
    gtk_box_pack_start(GTK_BOX(network_section), ip_label, FALSE, FALSE, 0);
    ip_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(ip_entry), "192.168.1.20"); // valor padrão
    gtk_box_pack_start(GTK_BOX(network_section), ip_entry, FALSE, FALSE, 0);

    GtkWidget *port_label = gtk_label_new("🔌 Porta do servidor:");
    gtk_box_pack_start(GTK_BOX(network_section), port_label, FALSE, FALSE, 0);
    port_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(port_entry), "8080"); // valor padrão
    gtk_box_pack_start(GTK_BOX(network_section), port_entry, FALSE, FALSE, 0);

    // Seção de arquivo
    GtkWidget *file_section = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_style_context_add_class(gtk_widget_get_style_context(file_section), "file-section");
    gtk_box_pack_start(GTK_BOX(main_box), file_section, FALSE, FALSE, 0);

    GtkWidget *file_label = gtk_label_new("📁 Selecionar Arquivo:");
    gtk_box_pack_start(GTK_BOX(file_section), file_label, FALSE, FALSE, 0);
    file_chooser_button = gtk_file_chooser_button_new("Escolher arquivo", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_box_pack_start(GTK_BOX(file_section), file_chooser_button, FALSE, FALSE, 0);

    // Botão processar
    process_button = gtk_button_new_with_label("🚀 Processar Arquivo");
    gtk_style_context_add_class(gtk_widget_get_style_context(process_button), "process-button");
    g_signal_connect(process_button, "clicked", G_CALLBACK(on_process_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(main_box), process_button, FALSE, FALSE, 0);

    // Seção de resultados
    GtkWidget *result_section = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_style_context_add_class(gtk_widget_get_style_context(result_section), "result-section");
    gtk_box_pack_start(GTK_BOX(main_box), result_section, TRUE, TRUE, 0);

    GtkWidget *result_title = gtk_label_new("📈 Resultados");
    gtk_style_context_add_class(gtk_widget_get_style_context(result_title), "result-title");
    gtk_box_pack_start(GTK_BOX(result_section), result_title, FALSE, FALSE, 0);

    GtkWidget *letras_card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_style_context_add_class(gtk_widget_get_style_context(letras_card), "result-card");
    gtk_box_pack_start(GTK_BOX(result_section), letras_card, FALSE, FALSE, 0);
    GtkWidget *letras_label = gtk_label_new("🔤 Letras encontradas:");
    gtk_style_context_add_class(gtk_widget_get_style_context(letras_label), "result-label");
    gtk_box_pack_start(GTK_BOX(letras_card), letras_label, FALSE, FALSE, 0);
    letras_value = gtk_label_new("--");
    gtk_style_context_add_class(gtk_widget_get_style_context(letras_value), "result-value");
    gtk_box_pack_start(GTK_BOX(letras_card), letras_value, FALSE, FALSE, 0);

    GtkWidget *numeros_card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_style_context_add_class(gtk_widget_get_style_context(numeros_card), "result-card");
    gtk_box_pack_start(GTK_BOX(result_section), numeros_card, FALSE, FALSE, 0);
    GtkWidget *numeros_label = gtk_label_new("🔢 Números encontrados:");
    gtk_style_context_add_class(gtk_widget_get_style_context(numeros_label), "result-label");
    gtk_box_pack_start(GTK_BOX(numeros_card), numeros_label, FALSE, FALSE, 0);
    numeros_value = gtk_label_new("--");
    gtk_style_context_add_class(gtk_widget_get_style_context(numeros_value), "result-value");
    gtk_box_pack_start(GTK_BOX(numeros_card), numeros_value, FALSE, FALSE, 0);

    // Status
    status_label = gtk_label_new("💡 Selecione IP, porta e arquivo para começar");
    gtk_style_context_add_class(gtk_widget_get_style_context(status_label), "status-label");
    gtk_box_pack_start(GTK_BOX(main_box), status_label, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
