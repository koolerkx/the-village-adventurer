from PIL import Image
import os

def read_png_paths(assets_path, config_file_):
    # path concat
    config_file = os.path.join(assets_path, config_file_)
    try:
        with open(config_file, 'r', encoding='sjis') as f:
            # Read lines, strip whitespace, and filter out empty lines or comments
            paths = [os.path.join(assets_path, line.strip()) for line in f if line.strip() and not line.strip().startswith('#')]
        return paths
    except FileNotFoundError:
        raise FileNotFoundError(f"Config file not found: {config_file}")
    except Exception as e:
        raise Exception(f"Error reading config file: {e}")

def concat_pngs(png_file_paths, output_path="assets_concat.png"):
    try:
        # Load all images into memory
        images = []
        for file_path in png_file_paths:
            if not os.path.exists(file_path):
                raise FileNotFoundError(f"File not found: {file_path}")
            img = Image.open(file_path).convert('RGBA')  # Ensure all images have alpha channel
            images.append(img)
        
        if not images:
            raise ValueError("No valid images provided.")
        
        # Get maximum width and total height
        max_width = max(img.width for img in images)
        total_height = sum(img.height for img in images)
        
        # Create new blank image with max width and total height
        new_image = Image.new('RGBA', (max_width, total_height))
        
        # Paste each image below the previous one, aligned to top-left
        current_y = 0
        for img in images:
            new_image.paste(img, (0, current_y))  # Align to top-left (x=0)
            current_y += img.height
        
        # Save the result to the specified output path
        new_image.save(output_path, format='PNG')
        print(f"Concatenated image saved successfully to {output_path}")
    
    except Exception as e:
        print(f"Error: {e}")

# Example usage
if __name__ == "__main__":
    config_file = "assets.config"
    output_file = "map_assets.png"
    assets_path = "assets"
    
    # Read PNG file paths from config file
    try:
        png_files = read_png_paths(assets_path, config_file)
        # Run the concatenation
        concat_pngs(png_files, output_file)
    except Exception as e:
        print(f"Error: {e}")