/*
 * ============================================================================
 *                        GTK3 - NOTAS DE REFERENCIA
 * ============================================================================
 *
 *  Archivo de referencia personal. Extensión .c para resaltado de sintaxis.
 *  No se compila.
 *
 *  GTK es una librería de GUI en modo retenido: creas widgets una vez,
 *  los conectas a señales (callbacks) y GTK maneja el event loop.
 *
 *  Dependencia: pkg-config --cflags --libs gtk+-3.0
 */

#include <gtk/gtk.h>

/* ============================================================================
 *  CONCEPTO BASICO
 * ============================================================================ */

/*
 * GTK funciona con un modelo de SEÑALES y CALLBACKS:
 *
 *   1. Creas un widget (botón, ventana, etc.)
 *   2. Conectas una señal a una función callback
 *   3. Ejecutas el main loop de GTK (gtk_main)
 *   4. GTK llama a tus callbacks cuando ocurren eventos
 *
 * Diferencia con Nuklear (modo inmediato):
 *   Nuklear: defines toda la UI cada frame, sin estado persistente.
 *   GTK:     creas widgets una vez, GTK los mantiene y redibuja.
 */

/* ============================================================================
 *  INICIALIZACION Y MAIN LOOP
 * ============================================================================ */

/*
 * gtk_init(&argc, &argv)
 * Inicializa GTK. Debe llamarse antes de cualquier otra función GTK.
 * Parsea argumentos de línea de comandos de GTK (--display, etc).
 * Se puede pasar NULL, NULL si no interesa.
 */
gtk_init(NULL, NULL);

/*
 * gtk_main()
 * Inicia el event loop de GTK. BLOQUEA hasta que se llame gtk_main_quit().
 * Todo lo que pasa después de gtk_main() se ejecuta al cerrar la app.
 */
gtk_main();

/*
 * gtk_main_quit()
 * Termina el event loop. Se llama normalmente desde un callback
 * (por ejemplo, al cerrar la ventana).
 */
gtk_main_quit();

/*
 * IMPORTANTE: gtk_main() bloquea el hilo. Si usas GTK junto con SDL,
 * NO podes correr ambos main loops en el mismo hilo.
 *
 * Para diálogos puntuales (file chooser, message box) sin gtk_main(),
 * ver la sección "USO CON SDL" más abajo.
 */

/* ============================================================================
 *  VENTANAS
 * ============================================================================ */

/*
 * gtk_window_new(type)
 * Crea una ventana nueva.
 *
 *   GTK_WINDOW_TOPLEVEL    Ventana normal del escritorio
 *   GTK_WINDOW_POPUP       Sin borde ni barra (tooltips, menús)
 */
GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

/*
 * Configuración básica de ventana:
 */
gtk_window_set_title(GTK_WINDOW(window), "Mi Aplicación");
gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

/*
 * Posiciones de ventana:
 *   GTK_WIN_POS_NONE              El SO decide
 *   GTK_WIN_POS_CENTER            Centrada en pantalla
 *   GTK_WIN_POS_MOUSE             Donde está el mouse
 *   GTK_WIN_POS_CENTER_ALWAYS     Siempre centrada (incluso al resize)
 */

/*
 * gtk_widget_show_all(widget)
 * Muestra el widget y todos sus hijos. Sin esto, nada aparece.
 */
gtk_widget_show_all(window);

/*
 * gtk_widget_destroy(widget)
 * Destruye un widget y todos sus hijos.
 */
gtk_widget_destroy(window);

/* ============================================================================
 *  SEÑALES Y CALLBACKS
 * ============================================================================ */

/*
 * g_signal_connect(widget, "señal", callback, data)
 * Conecta una señal de un widget a una función callback.
 *
 *   widget:   El widget que emite la señal.
 *   "señal":  Nombre de la señal (string).
 *   callback: Función que se ejecuta (casteada con G_CALLBACK).
 *   data:     Puntero extra que se pasa al callback (NULL si no se usa).
 */

/* Ejemplo: cerrar ventana */
void on_destroy(GtkWidget *widget, gpointer data)
{
    (void)widget;
    (void)data;
    gtk_main_quit();
}
g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

/* Ejemplo: click en botón */
void on_button_click(GtkWidget *button, gpointer data)
{
    (void)button;
    const char *mensaje = (const char *)data;
    g_print("Botón clickeado: %s\n", mensaje);
}
g_signal_connect(button, "clicked", G_CALLBACK(on_button_click), "Hola");

/*
 * Señales comunes:
 *
 *   "destroy"           Widget destruido (ventana cerrada)
 *   "delete-event"      Antes de cerrar (retornar TRUE cancela el cierre)
 *   "clicked"           Botón clickeado
 *   "toggled"           CheckButton/RadioButton cambiado
 *   "changed"           Entry/ComboBox modificado
 *   "value-changed"     SpinButton/Scale cambió de valor
 *   "activate"          Enter presionado en un Entry
 *   "response"          Diálogo respondido
 */

/* ============================================================================
 *  CONTENEDORES
 * ============================================================================ */

/*
 * En GTK los widgets se organizan en contenedores.
 * Una ventana solo puede tener UN hijo directo, así que se usa un
 * contenedor para meter múltiples widgets.
 */

/* --- GtkBox: alinea widgets en fila o columna --- */
/*
 * gtk_box_new(orientation, spacing)
 *
 *   GTK_ORIENTATION_VERTICAL     Columna (arriba a abajo)
 *   GTK_ORIENTATION_HORIZONTAL   Fila (izquierda a derecha)
 *   spacing: píxeles entre widgets
 */
GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

/*
 * gtk_box_pack_start(box, child, expand, fill, padding)
 *
 *   expand:  TRUE = el widget usa espacio extra disponible
 *   fill:    TRUE = el widget se estira para llenar su espacio
 *   padding: píxeles extra alrededor del widget
 */
gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

/*
 * gtk_container_add(container, widget)
 * Forma genérica de agregar un hijo a cualquier contenedor.
 * Para Box, equivale a pack_start con expand=fill=TRUE, padding=0.
 */
gtk_container_add(GTK_CONTAINER(window), vbox);

/* --- GtkGrid: grilla de filas y columnas --- */
GtkWidget *grid = gtk_grid_new();
gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

/*
 * gtk_grid_attach(grid, widget, col, row, width, height)
 * Coloca un widget en la grilla.
 *
 *   col, row:       Posición (0-indexed)
 *   width, height:  Cuántas celdas ocupa
 */
gtk_grid_attach(GTK_GRID(grid), label,  0, 0, 1, 1);  // (0,0) 1x1
gtk_grid_attach(GTK_GRID(grid), entry,  1, 0, 2, 1);  // (1,0) 2x1
gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 3, 1);  // (0,1) 3x1

/* ============================================================================
 *  WIDGETS PRINCIPALES
 * ============================================================================ */

/* --- Label --- */
GtkWidget *label = gtk_label_new("Texto simple");
gtk_label_set_text(GTK_LABEL(label), "Nuevo texto");
const char *text = gtk_label_get_text(GTK_LABEL(label));

/* Label con markup (HTML-like) */
gtk_label_set_markup(GTK_LABEL(label),
    "<b>Negrita</b>, <i>cursiva</i>, <span color='red'>rojo</span>");

/* --- Button --- */
GtkWidget *button = gtk_button_new_with_label("Click me");

/* --- Entry (campo de texto) --- */
GtkWidget *entry = gtk_entry_new();
gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Escribí algo...");
gtk_entry_set_max_length(GTK_ENTRY(entry), 50);
const char *input = gtk_entry_get_text(GTK_ENTRY(entry));

/* Entry de contraseña */
gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);  // Oculta caracteres

/* --- CheckButton --- */
GtkWidget *check = gtk_check_button_new_with_label("Fullscreen");
gboolean active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), TRUE);

/* --- RadioButton (grupo mutuamente excluyente) --- */
GtkWidget *radio1 = gtk_radio_button_new_with_label(NULL, "Opción A");
GtkWidget *radio2 = gtk_radio_button_new_with_label_from_widget(
    GTK_RADIO_BUTTON(radio1), "Opción B");
GtkWidget *radio3 = gtk_radio_button_new_with_label_from_widget(
    GTK_RADIO_BUTTON(radio1), "Opción C");
/* El primero define el grupo; los demás se unen a él. */

/* --- SpinButton (campo numérico con flechas) --- */
/*
 * gtk_spin_button_new_with_range(min, max, step)
 */
GtkWidget *spin = gtk_spin_button_new_with_range(0, 100, 1);
gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), 60);
int val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));

/* --- Scale (slider) --- */
GtkWidget *scale = gtk_scale_new_with_range(
    GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);  // Muestra el número
double pos = gtk_range_get_value(GTK_RANGE(scale));

/* --- ComboBoxText (dropdown) --- */
GtkWidget *combo = gtk_combo_box_text_new();
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Opción A");
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Opción B");
gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Opción C");
gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);  // Seleccionar primera

/* Obtener selección */
char *selected = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
g_free(selected);  // Siempre liberar con g_free

/* --- ProgressBar --- */
GtkWidget *progress = gtk_progress_bar_new();
gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0.75);  // 75%
gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "75%");
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress), TRUE);

/* --- Separator --- */
GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

/* --- Image --- */
GtkWidget *img = gtk_image_new_from_file("assets/sprites/logo.png");

/* ============================================================================
 *  DIALOGOS
 * ============================================================================ */

/*
 * Los diálogos son ventanas modales que bloquean la interacción con
 * la ventana padre hasta que el usuario responde.
 *
 * gtk_dialog_run(dialog) muestra el diálogo y BLOQUEA hasta que
 * el usuario lo cierra. Retorna un GTK_RESPONSE_*.
 */

/* --- Message Dialog (alerta/confirmación) --- */
/*
 * gtk_message_dialog_new(parent, flags, type, buttons, format, ...)
 *
 * Tipos de mensaje:
 *   GTK_MESSAGE_INFO       Información (ícono i)
 *   GTK_MESSAGE_WARNING    Advertencia (ícono !)
 *   GTK_MESSAGE_QUESTION   Pregunta (ícono ?)
 *   GTK_MESSAGE_ERROR      Error (ícono X)
 *
 * Tipos de botones:
 *   GTK_BUTTONS_OK             Solo OK
 *   GTK_BUTTONS_CLOSE          Solo Cerrar
 *   GTK_BUTTONS_YES_NO         Sí / No
 *   GTK_BUTTONS_OK_CANCEL      OK / Cancelar
 *   GTK_BUTTONS_NONE           Sin botones (agregar manualmente)
 */
GtkWidget *dialog = gtk_message_dialog_new(
    GTK_WINDOW(window),                   // Ventana padre (o NULL)
    GTK_DIALOG_MODAL,                     // Flags
    GTK_MESSAGE_WARNING,                  // Tipo
    GTK_BUTTONS_YES_NO,                   // Botones
    "¿Estás seguro de que querés salir?"  // Texto (formato printf)
);
gtk_window_set_title(GTK_WINDOW(dialog), "Confirmar");

int response = gtk_dialog_run(GTK_DIALOG(dialog));
switch (response) {
    case GTK_RESPONSE_YES:
        g_print("Usuario dijo Sí\n");
        break;
    case GTK_RESPONSE_NO:
        g_print("Usuario dijo No\n");
        break;
}
gtk_widget_destroy(dialog);

/*
 * Responses comunes:
 *   GTK_RESPONSE_YES        Sí
 *   GTK_RESPONSE_NO         No
 *   GTK_RESPONSE_OK         OK
 *   GTK_RESPONSE_CANCEL     Cancelar
 *   GTK_RESPONSE_CLOSE      Cerrar
 *   GTK_RESPONSE_DELETE_EVENT  El usuario cerró con la X
 */

/* --- File Chooser Dialog (abrir/guardar archivo) --- */
/*
 * Acciones:
 *   GTK_FILE_CHOOSER_ACTION_OPEN            Abrir archivo
 *   GTK_FILE_CHOOSER_ACTION_SAVE            Guardar archivo
 *   GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER   Seleccionar carpeta
 */
GtkWidget *chooser = gtk_file_chooser_dialog_new(
    "Abrir archivo",                       // Título
    GTK_WINDOW(window),                    // Padre
    GTK_FILE_CHOOSER_ACTION_OPEN,          // Acción
    "Cancelar", GTK_RESPONSE_CANCEL,       // Botón 1
    "Abrir",    GTK_RESPONSE_ACCEPT,       // Botón 2
    NULL                                   // Terminar con NULL
);

/* Filtro de archivos */
GtkFileFilter *filter = gtk_file_filter_new();
gtk_file_filter_set_name(filter, "Imágenes PNG");
gtk_file_filter_add_pattern(filter, "*.png");
gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

/* Filtro para todos los archivos */
GtkFileFilter *all = gtk_file_filter_new();
gtk_file_filter_set_name(all, "Todos los archivos");
gtk_file_filter_add_pattern(all, "*");
gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), all);

/* Ejecutar y obtener resultado */
if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_ACCEPT)
{
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
    g_print("Archivo seleccionado: %s\n", filename);
    g_free(filename);  // Siempre liberar con g_free
}
gtk_widget_destroy(chooser);

/* --- Color Chooser Dialog --- */
GtkWidget *color_dialog = gtk_color_chooser_dialog_new(
    "Elegir color", GTK_WINDOW(window));

if (gtk_dialog_run(GTK_DIALOG(color_dialog)) == GTK_RESPONSE_OK)
{
    GdkRGBA color;
    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(color_dialog), &color);
    g_print("Color: R=%.0f G=%.0f B=%.0f A=%.0f\n",
            color.red * 255, color.green * 255,
            color.blue * 255, color.alpha * 255);
}
gtk_widget_destroy(color_dialog);

/* ============================================================================
 *  USO CON SDL (SIN gtk_main)
 * ============================================================================ */

/*
 * Cuando usas GTK solo para diálogos dentro de una app SDL,
 * NO necesitas gtk_main(). Los diálogos con gtk_dialog_run()
 * manejan su propio mini event loop.
 *
 * PERO: necesitas procesar los eventos pendientes de GTK
 * antes y después de los diálogos.
 */

/*
 * Patrón para usar un diálogo GTK desde SDL:
 */
void mostrar_dialogo_desde_sdl(void)
{
    /* 1. Inicializar GTK (una sola vez, al inicio del programa) */
    // gtk_init(NULL, NULL);  // Ya se hizo al inicio

    /* 2. Crear y ejecutar el diálogo */
    GtkWidget *dialog = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK, "Mensaje desde SDL");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    /* 3. Procesar eventos GTK pendientes */
    while (gtk_events_pending())
        gtk_main_iteration();
}

/*
 * Ejemplo: File chooser desde SDL
 */
char *abrir_archivo_sdl(void)
{
    GtkWidget *chooser = gtk_file_chooser_dialog_new(
        "Abrir", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancelar", GTK_RESPONSE_CANCEL,
        "Abrir",    GTK_RESPONSE_ACCEPT,
        NULL);

    char *result = NULL;
    if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_ACCEPT)
    {
        result = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
        // NOTA: result fue alocado con g_malloc, liberar con g_free
    }

    gtk_widget_destroy(chooser);
    while (gtk_events_pending())
        gtk_main_iteration();

    return result;  // El caller debe llamar g_free(result)
}

/*
 * IMPORTANTE sobre SDL + GTK:
 *
 *   - Llamar gtk_init(NULL, NULL) al inicio del programa, ANTES de
 *     SDL_Init(). GTK configura cosas del display que SDL hereda.
 *
 *   - gtk_dialog_run() bloquea el hilo. Mientras el diálogo está
 *     abierto, el game loop de SDL no corre (la ventana se congela).
 *     Para diálogos rápidos (abrir archivo, confirmar) está bien.
 *
 *   - Siempre llamar while(gtk_events_pending()) gtk_main_iteration()
 *     después de cerrar un diálogo para limpiar eventos residuales.
 *
 *   - Los strings de GTK se liberan con g_free(), NO con free().
 */

/* ============================================================================
 *  GLIB: TIPOS Y FUNCIONES UTILES
 * ============================================================================ */

/*
 * GTK viene con GLib, que provee tipos y utilidades. Algunos útiles:
 */

/* --- Tipos --- */
/*
 * gboolean    Booleano (TRUE / FALSE)
 * gint        int
 * guint       unsigned int
 * gchar       char
 * gpointer    void*
 * gsize       size_t
 */

/* --- Strings --- */
gchar *str = g_strdup("copia");                         // strdup
gchar *fmt = g_strdup_printf("FPS: %d", 60);            // sprintf con alloc
gchar *joined = g_strjoin("/", "assets", "img", NULL);  // join con separador
g_free(str);
g_free(fmt);
g_free(joined);

/* --- Print --- */
g_print("Mensaje normal\n");          // Equivale a printf
g_printerr("Error: algo falló\n");    // Equivale a fprintf(stderr, ...)

/* --- Memory --- */
gpointer mem = g_malloc(1024);     // malloc (aborta si falla, nunca NULL)
gpointer mem0 = g_malloc0(1024);   // calloc (inicializa a 0)
g_free(mem);
g_free(mem0);

/* ============================================================================
 *  CSS STYLING
 * ============================================================================ */

/*
 * GTK3 usa CSS para estilos visuales, similar a HTML/CSS.
 * Se puede cargar desde un string o archivo.
 */

/* Desde string */
GtkCssProvider *css = gtk_css_provider_new();
gtk_css_provider_load_from_data(css,
    "window { background-color: #2d2d2d; }"
    "button { color: white; background: #444; border-radius: 4px; }"
    "label  { color: #ddd; font-size: 14px; }",
    -1, NULL);

gtk_style_context_add_provider_for_screen(
    gdk_screen_get_default(),
    GTK_STYLE_PROVIDER(css),
    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

g_object_unref(css);

/* Desde archivo */
GtkCssProvider *css_file = gtk_css_provider_new();
gtk_css_provider_load_from_path(css_file, "assets/style.css", NULL);

/* Aplicar clase CSS a un widget específico */
GtkStyleContext *context = gtk_widget_get_style_context(button);
gtk_style_context_add_class(context, "mi-boton-especial");
/* En el CSS: .mi-boton-especial { background: red; } */

/* ============================================================================
 *  EJEMPLO COMPLETO: APP MINIMA
 * ============================================================================ */

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    /* Ventana */
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Demo");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* Layout vertical */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Label */
    GtkWidget *label = gtk_label_new("Hola GTK!");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    /* Entry */
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Escribí tu nombre");
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    /* Botón */
    GtkWidget *button = gtk_button_new_with_label("Saludar");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_click), entry);

    /* Mostrar y correr */
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

/* ============================================================================
 *  EJEMPLO: DIALOGOS UTILES PARA EL ENGINE
 * ============================================================================ */

/*
 * Funciones wrapper listas para usar desde el engine SDL.
 * Llamar gtk_init(NULL, NULL) una sola vez al inicio.
 */

/* Mostrar error fatal */
void engine_error_dialog(const char *title, const char *message)
{
    GtkWidget *d = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(d), title);
    gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
    while (gtk_events_pending()) gtk_main_iteration();
}

/* Confirmar acción (retorna TRUE si el usuario dijo Sí) */
gboolean engine_confirm_dialog(const char *message)
{
    GtkWidget *d = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO, "%s", message);
    int response = gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
    while (gtk_events_pending()) gtk_main_iteration();
    return (response == GTK_RESPONSE_YES);
}

/* Seleccionar archivo (retorna path alocado con g_malloc, o NULL) */
char *engine_open_file(const char *title, const char *filter_name,
                       const char *pattern)
{
    GtkWidget *d = gtk_file_chooser_dialog_new(
        title, NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancelar", GTK_RESPONSE_CANCEL,
        "Abrir",    GTK_RESPONSE_ACCEPT,
        NULL);

    if (filter_name && pattern)
    {
        GtkFileFilter *f = gtk_file_filter_new();
        gtk_file_filter_set_name(f, filter_name);
        gtk_file_filter_add_pattern(f, pattern);
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(d), f);
    }

    char *result = NULL;
    if (gtk_dialog_run(GTK_DIALOG(d)) == GTK_RESPONSE_ACCEPT)
        result = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(d));

    gtk_widget_destroy(d);
    while (gtk_events_pending()) gtk_main_iteration();
    return result;
}

/* ============================================================================
 *  CASTING EN GTK
 * ============================================================================ */

/*
 * GTK usa macros de casting con verificación de tipo en runtime.
 * Siempre usá estas macros en vez de (cast):
 *
 *   GTK_WINDOW(widget)          -> GtkWindow*
 *   GTK_BUTTON(widget)          -> GtkButton*
 *   GTK_LABEL(widget)           -> GtkLabel*
 *   GTK_ENTRY(widget)           -> GtkEntry*
 *   GTK_BOX(widget)             -> GtkBox*
 *   GTK_GRID(widget)            -> GtkGrid*
 *   GTK_CONTAINER(widget)       -> GtkContainer*
 *   GTK_TOGGLE_BUTTON(widget)   -> GtkToggleButton*
 *   GTK_COMBO_BOX(widget)       -> GtkComboBox*
 *   GTK_COMBO_BOX_TEXT(widget)  -> GtkComboBoxText*
 *   GTK_FILE_CHOOSER(widget)    -> GtkFileChooser*
 *   GTK_DIALOG(widget)          -> GtkDialog*
 *   GTK_RANGE(widget)           -> GtkRange*
 *   GTK_SPIN_BUTTON(widget)     -> GtkSpinButton*
 *   GTK_SCALE(widget)           -> GtkScale*
 *   GTK_COLOR_CHOOSER(widget)   -> GtkColorChooser*
 *
 * Si el tipo no coincide, GTK imprime un warning en runtime.
 * Todos los widgets son GtkWidget* en su forma base.
 */

/* ============================================================================
 *  JERARQUIA DE WIDGETS (simplificada)
 * ============================================================================ */

/*
 * GtkWidget
 *  ├── GtkContainer
 *  │    ├── GtkBin (un solo hijo)
 *  │    │    ├── GtkWindow
 *  │    │    ├── GtkButton
 *  │    │    ├── GtkFrame
 *  │    │    └── GtkScrolledWindow
 *  │    ├── GtkBox
 *  │    └── GtkGrid
 *  ├── GtkLabel
 *  ├── GtkEntry
 *  ├── GtkImage
 *  ├── GtkProgressBar
 *  ├── GtkScale
 *  ├── GtkSpinButton
 *  └── GtkSeparator
 */
