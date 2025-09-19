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
        gtk_label_set_text(GTK_LABEL(status_label), "Selecione um arquivo");
        return;
    }

    gtk_label_set_text(GTK_LABEL(status_label), "Processando...");
    gtk_widget_set_sensitive(process_button, FALSE);

    // Atualizar interface
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }

    std::string conteudo = lerArquivo(filename);
    if (conteudo.empty()) {
        gtk_label_set_text(GTK_LABEL(status_label), "Erro ao ler arquivo");
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
            gtk_label_set_text(GTK_LABEL(status_label), "✓ Processado com sucesso");
        } catch (const std::exception& e) {
            gtk_label_set_text(GTK_LABEL(status_label), "Erro na resposta");
        }
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Erro de conexão");
    }
    
    gtk_widget_set_sensitive(process_button, TRUE);
    g_free(filename);
}

void apply_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    const gchar *css_data = R"(
        /* Janela principal */
        window { 
            background: linear-gradient(135deg, #e0f2fe 0%, #b3e5fc 100%);
        }
        
        /* Container principal */
        .main-container { 
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(20px);
            border-radius: 16px;
            padding: 20px;
            margin: 16px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
        }
        
        /* Header compacto */
        .header {
            text-align: center;
            margin-bottom: 20px;
        }
        
        .title { 
            font-size: 20px;
            font-weight: 700;
            color: #2d3748;
            margin-bottom: 4px;
        }
        
        .subtitle { 
            font-size: 12px;
            color: #718096;
        }
        
        /* Grid de configuração */
        .config-grid {
            margin-bottom: 16px;
        }
        
        .config-row {
            margin-bottom: 12px;
        }
        
        /* Input moderno */
        entry {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 13px;
            background: #ffffff;
            color: #2d3748;
            transition: all 0.2s ease;
        }
        
        entry:focus {
            border-color: #29b6f6;
            box-shadow: 0 0 0 3px rgba(41, 182, 246, 0.1);
            outline: none;
        }
        
        /* Labels compactos */
        .input-label {
            font-size: 11px;
            font-weight: 600;
            color: #4a5568;
            margin-bottom: 4px;
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        
        /* Botão de arquivo */
        .file-chooser-button {
            border: 2px dashed #cbd5e0;
            border-radius: 8px;
            padding: 12px;
            background: #f7fafc;
            font-size: 12px;
            color: #4a5568;
            transition: all 0.2s ease;
        }
        
        .file-chooser-button:hover {
            background: #e1f5fe;
            border-color: #81d4fa;
        }
        
        /* Botão processar moderno */
        .process-button { 
            background: linear-gradient(135deg, #29b6f6 0%, #039be5 100%);
            color: #ffffff;
            border: none;
            border-radius: 8px;
            padding: 12px 24px;
            font-size: 13px;
            font-weight: 600;
            margin: 16px 0 12px 0;
            min-height: 40px;
            transition: all 0.3s ease;
            box-shadow: 0 4px 15px rgba(41, 182, 246, 0.4);
        }
        
        .process-button:hover { 
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(41, 182, 246, 0.6);
        }
        
        .process-button:active {
            transform: translateY(0px);
        }
        
        .process-button:disabled {
            background: #a0aec0;
            box-shadow: none;
            transform: none;
        }
        
        /* Resultados em linha */
        .results-row {
            margin: 12px 0;
        }
        
        .result-card { 
            background: linear-gradient(135deg, #f1f8fc 0%, #e1f5fe 100%);
            border-radius: 8px;
            padding: 12px 16px;
            margin: 0 4px;
            border: 1px solid #b3e5fc;
            text-align: center;
            min-width: 100px;
        }
        
        .result-label { 
            font-size: 10px;
            font-weight: 600;
            color: #718096;
            text-transform: uppercase;
            letter-spacing: 0.5px;
            margin-bottom: 2px;
        }
        
        .result-value { 
            font-size: 24px;
            font-weight: 800;
            color: #0277bd;
            line-height: 1;
        }
        
        /* Status compacto */
        .status-label { 
            font-size: 11px;
            padding: 8px 12px;
            border-radius: 6px;
            text-align: center;
            background: #e1f5fe;
            color: #01579b;
            border: 1px solid #b3e5fc;
            font-weight: 500;
        }
        
        /* Footer minimalista */
        .footer {
            margin-top: 16px;
            text-align: center;
            opacity: 0.6;
        }
        
        /* Separadores */
        .section-separator {
            height: 1px;
            background: linear-gradient(90deg, transparent, #e2e8f0, transparent);
            margin: 16px 0;
        }
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

    // Janela principal compacta
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Contador de Caracteres");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 480);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Container principal
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(main_box), "main-container");
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Header compacto
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(header_box), "header");
    gtk_box_pack_start(GTK_BOX(main_box), header_box, FALSE, FALSE, 0);

    GtkWidget *title = gtk_label_new("Contador de Caracteres");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "title");
    gtk_box_pack_start(GTK_BOX(header_box), title, FALSE, FALSE, 0);

    GtkWidget *subtitle = gtk_label_new("Análise rápida de arquivos de texto");
    gtk_style_context_add_class(gtk_widget_get_style_context(subtitle), "subtitle");
    gtk_box_pack_start(GTK_BOX(header_box), subtitle, FALSE, FALSE, 0);

    // Separador
    GtkWidget *separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_style_context_add_class(gtk_widget_get_style_context(separator1), "section-separator");
    gtk_box_pack_start(GTK_BOX(main_box), separator1, FALSE, FALSE, 0);

    // Grid de configuração de rede (2 colunas)
    GtkWidget *network_grid = gtk_grid_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(network_grid), "config-grid");
    gtk_grid_set_column_spacing(GTK_GRID(network_grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(network_grid), 8);
    gtk_box_pack_start(GTK_BOX(main_box), network_grid, FALSE, FALSE, 0);

    // IP
    GtkWidget *ip_label = gtk_label_new("Servidor");
    gtk_style_context_add_class(gtk_widget_get_style_context(ip_label), "input-label");
    gtk_widget_set_halign(ip_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(network_grid), ip_label, 0, 0, 1, 1);
    
    ip_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(ip_entry), "192.168.1.20");
    gtk_entry_set_placeholder_text(GTK_ENTRY(ip_entry), "IP do servidor");
    gtk_grid_attach(GTK_GRID(network_grid), ip_entry, 0, 1, 1, 1);

    // Porta
    GtkWidget *port_label = gtk_label_new("Porta");
    gtk_style_context_add_class(gtk_widget_get_style_context(port_label), "input-label");
    gtk_widget_set_halign(port_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(network_grid), port_label, 1, 0, 1, 1);
    
    port_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(port_entry), "8080");
    gtk_entry_set_placeholder_text(GTK_ENTRY(port_entry), "Porta");
    gtk_widget_set_size_request(port_entry, 80, -1);
    gtk_grid_attach(GTK_GRID(network_grid), port_entry, 1, 1, 1, 1);

    // Separador
    GtkWidget *separator2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_style_context_add_class(gtk_widget_get_style_context(separator2), "section-separator");
    gtk_box_pack_start(GTK_BOX(main_box), separator2, FALSE, FALSE, 0);

    // Arquivo
    GtkWidget *file_label = gtk_label_new("Arquivo");
    gtk_style_context_add_class(gtk_widget_get_style_context(file_label), "input-label");
    gtk_widget_set_halign(file_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(main_box), file_label, FALSE, FALSE, 0);

    file_chooser_button = gtk_file_chooser_button_new("📄 Escolher arquivo", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_style_context_add_class(gtk_widget_get_style_context(file_chooser_button), "file-chooser-button");
    gtk_box_pack_start(GTK_BOX(main_box), file_chooser_button, FALSE, FALSE, 0);

    // Botão processar
    process_button = gtk_button_new_with_label("🚀 Processar");
    gtk_style_context_add_class(gtk_widget_get_style_context(process_button), "process-button");
    g_signal_connect(process_button, "clicked", G_CALLBACK(on_process_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(main_box), process_button, FALSE, FALSE, 0);

    // Resultados em linha
    GtkWidget *results_grid = gtk_grid_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(results_grid), "results-row");
    gtk_grid_set_column_spacing(GTK_GRID(results_grid), 8);
    gtk_grid_set_column_homogeneous(GTK_GRID(results_grid), TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), results_grid, FALSE, FALSE, 0);

    // Card de letras
    GtkWidget *letras_card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(letras_card), "result-card");
    gtk_grid_attach(GTK_GRID(results_grid), letras_card, 0, 0, 1, 1);
    
    GtkWidget *letras_label = gtk_label_new("Letras");
    gtk_style_context_add_class(gtk_widget_get_style_context(letras_label), "result-label");
    gtk_box_pack_start(GTK_BOX(letras_card), letras_label, FALSE, FALSE, 0);
    
    letras_value = gtk_label_new("—");
    gtk_style_context_add_class(gtk_widget_get_style_context(letras_value), "result-value");
    gtk_box_pack_start(GTK_BOX(letras_card), letras_value, FALSE, FALSE, 0);

    // Card de números
    GtkWidget *numeros_card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(numeros_card), "result-card");
    gtk_grid_attach(GTK_GRID(results_grid), numeros_card, 1, 0, 1, 1);
    
    GtkWidget *numeros_label = gtk_label_new("Números");
    gtk_style_context_add_class(gtk_widget_get_style_context(numeros_label), "result-label");
    gtk_box_pack_start(GTK_BOX(numeros_card), numeros_label, FALSE, FALSE, 0);
    
    numeros_value = gtk_label_new("—");
    gtk_style_context_add_class(gtk_widget_get_style_context(numeros_value), "result-value");
    gtk_box_pack_start(GTK_BOX(numeros_card), numeros_value, FALSE, FALSE, 0);

    // Status
    status_label = gtk_label_new("Pronto para processar");
    gtk_style_context_add_class(gtk_widget_get_style_context(status_label), "status-label");
    gtk_box_pack_start(GTK_BOX(main_box), status_label, FALSE, FALSE, 0);

    // Footer minimalista
    GtkWidget *footer_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_style_context_add_class(gtk_widget_get_style_context(footer_box), "footer");
    gtk_widget_set_halign(footer_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(main_box), footer_box, FALSE, FALSE, 0);
    
    // Logo pequena
    GtkWidget *logo_image = gtk_image_new_from_file("cliente/assets/images/quanan.png");
    if (logo_image) {
        gtk_widget_set_size_request(logo_image, 24, 24);
        gtk_box_pack_start(GTK_BOX(footer_box), logo_image, FALSE, FALSE, 0);
    }

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}