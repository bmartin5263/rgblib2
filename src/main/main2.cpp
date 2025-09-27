#include <LovyanGFX.hpp>

// Define custom LGFX class for GC9A01
class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      // SPI bus configuration
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;  // 40MHz
      cfg.freq_read = 16000000;   // 16MHz
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;

      // Pin configuration
      cfg.pin_sclk = 18;  // SCL
      cfg.pin_mosi = 23;  // SDA
      cfg.pin_miso = -1;  // Not used
      cfg.pin_dc = 2;     // DC

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      cfg.pin_cs = 5;      // CS
      cfg.pin_rst = 4;     // RST
      cfg.pin_busy = -1;   // Not used

      cfg.memory_width = 240;
      cfg.memory_height = 240;
      cfg.panel_width = 240;
      cfg.panel_height = 240;

      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = 15;      // Backlight pin
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};

// Create display instance
static LGFX lcd;

// Demo function - draw some basic shapes
void draw_demo()
{
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextSize(2);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawString("GC9A01 Demo", 40, 10);

  // Draw some colorful shapes
  lcd.fillCircle(120, 120, 80, TFT_RED);
  lcd.fillCircle(120, 120, 60, TFT_GREEN);
  lcd.fillCircle(120, 120, 40, TFT_BLUE);
  lcd.fillCircle(120, 120, 20, TFT_YELLOW);

  vTaskDelay(2000 / portTICK_PERIOD_MS);
}

// Draw animated circles
void animate_circles()
{
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawString("Circles", 80, 10);

  for (int i = 0; i < 5; i++) {
    for (int r = 10; r < 120; r += 10) {
      lcd.drawCircle(120, 120, r, random(0xFFFF));
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd.fillScreen(TFT_BLACK);
    lcd.drawString("Circles", 80, 10);
  }
}

// Draw rectangles
void draw_rectangles()
{
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_YELLOW);
  lcd.drawString("Rectangles", 60, 10);

  for (int i = 0; i < 20; i++) {
    int x = random(0, 200);
    int y = random(30, 200);
    int w = random(20, 60);
    int h = random(20, 60);
    uint16_t color = random(0xFFFF);

    lcd.fillRect(x, y, w, h, color);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// Draw lines
void draw_lines()
{
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_CYAN);
  lcd.drawString("Lines", 90, 10);

  // Radial lines from center
  for (int angle = 0; angle < 360; angle += 10) {
    float rad = angle * PI / 180.0;
    int x = 120 + (int)(100 * cos(rad));
    int y = 120 + (int)(100 * sin(rad));

    lcd.drawLine(120, 120, x, y, random(0xFFFF));
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// Draw triangles
void draw_triangles()
{
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_MAGENTA);
  lcd.drawString("Triangles", 70, 10);

  for (int i = 0; i < 15; i++) {
    int x0 = random(0, 240);
    int y0 = random(30, 240);
    int x1 = random(0, 240);
    int y1 = random(30, 240);
    int x2 = random(0, 240);
    int y2 = random(30, 240);

    lcd.fillTriangle(x0, y0, x1, y1, x2, y2, random(0xFFFF));
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// Rainbow gradient
void draw_gradient()
{
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawString("Gradient", 75, 10);

  for (int y = 30; y < 240; y++) {
    uint16_t color = lcd.color565(
      map(y, 30, 240, 255, 0),
      map(y, 30, 140, 0, 255),
      map(y, 140, 240, 0, 255)
    );
    lcd.drawFastHLine(0, y, 240, color);
  }

  vTaskDelay(2000 / portTICK_PERIOD_MS);
}

// Bouncing ball animation
void bouncing_ball()
{
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawString("Bouncing Ball", 50, 10);

  int x = 120, y = 120;
  int dx = 3, dy = 2;
  int radius = 15;

  for (int i = 0; i < 200; i++) {
    // Clear previous ball
    lcd.fillCircle(x, y, radius, TFT_BLACK);

    // Update position
    x += dx;
    y += dy;

    // Bounce off edges
    if (x - radius < 0 || x + radius > 240) dx = -dx;
    if (y - radius < 30 || y + radius > 240) dy = -dy;

    // Draw new ball
    lcd.fillCircle(x, y, radius, TFT_RED);
    lcd.drawCircle(x, y, radius, TFT_WHITE);

    vTaskDelay(20 / portTICK_PERIOD_MS);
  }

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

extern "C" void app_main(void)
{
  // Initialize the display
  lcd.init();
  lcd.setRotation(0);
  lcd.setBrightness(255);

  printf("LovyanGFX GC9A01 Demo Starting...\n");

  // Run demo loop
  while (1) {
    draw_demo();
    animate_circles();
    draw_rectangles();
    draw_lines();
    draw_triangles();
    draw_gradient();
    bouncing_ball();
  }
}