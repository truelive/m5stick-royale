import argparse

from PIL import Image


def convert_to_rgb565(image_path, asset_name, startX, startY, maxWeight, maxHeight):
    try:
        alpha_treshold = 0.5
        bg_fill_collor = 0xFFFF
        # Open the bitmap image
        bitmap = Image.open(image_path)

        # Convert image to RGB mode (in case it's in a different mode)
        bitmap = bitmap.convert("RGBA")
        bitmap = bitmap.crop((startX, startY, startX + maxWeight, startY + maxHeight))
        # Get image dimensions
        width, height = bitmap.size

        # Open a file to write RGB565 data
        img_name = image_path.rsplit(".", 1)[0]
        img_name = img_name.split("/")[-1].replace("-", "_")
        output_file = "include/assets/" + asset_name + ".h"
        output_png = "include/assets/" + asset_name + ".png"
        bitmap.save(output_png)
        with open(output_file, "w") as f:
            f.write("#include <Arduino.h>\n")
            f.write(f"const uint16_t {asset_name}_width = {str(maxWeight)};\n")
            f.write(f"const uint16_t {asset_name}_height= {str(maxHeight)};\n")
            f.write("\n")
            f.write(f"const uint16_t PROGMEM {asset_name}[] = ")
            f.write("{\n")
            for y in range(maxHeight):
                for x in range(maxWeight):
                    # Get RGB color tuple at pixel (x, y)
                    r, g, b, a = bitmap.getpixel((x, y))

                    # Convert RGB to RGB565
                    # // ________ ________ GGGBBBBB RRRRRGGG

                    rgb = to_rgb565(r, g, b)
                    rgb565 = swap_bytes(rgb)
                    if a < alpha_treshold:
                        rgb565 = bg_fill_collor
                    f.write(hex(rgb565))
                    f.write(",")
                f.write("\n")
            f.write("};")

        print(f"RGB565 data saved to {output_file}")

    except Exception as e:
        print(f"Error: {e}")


def to_rgb565(r, g, b):
    rgb = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
    return rgb


def swap_bytes(rgb565):
    swap_string_low = rgb565 >> 8
    swap_string_high = (rgb565 & 0x00FF) << 8
    swap_string = swap_string_low | swap_string_high
    return swap_string


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert a bitmap image to RGB565 format."
    )
    parser.add_argument("asset_name", type=str, help="Name of the asset")
    parser.add_argument("image_path", type=str, help="Path to the bitmap image")
    parser.add_argument("start_x", type=int, help="start x position")
    parser.add_argument("start_y", type=int, help="start y position")
    parser.add_argument("max_width", type=int, help="max width")
    parser.add_argument("max_height", type=int, help="max height")

    args = parser.parse_args()
    convert_to_rgb565(args.image_path, args.asset_name, args.start_x, args.start_y, args.max_width, args.max_height)
