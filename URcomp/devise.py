from PIL import Image
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import A4
import math
import os

# --- Paramètres ---
input_image = "Part1.DXF.png"   # Ton image PNG (1000mm x 1000mm à 300 dpi)
output_pdf = "Maquette.pdf"
dpi = 300  # Résolution de l'image (300 recommandé pour l'impression)

# Dimensions A4 en mm et conversion en pixels
A4_WIDTH_MM = 210
A4_HEIGHT_MM = 297
A4_WIDTH_PX = int(A4_WIDTH_MM / 25.4 * dpi)
A4_HEIGHT_PX = int(A4_HEIGHT_MM / 25.4 * dpi)

# --- Charger l'image ---
img = Image.open(input_image)
img_width, img_height = img.size

# --- Calcul du nombre de pages ---
cols = math.ceil(img_width / A4_WIDTH_PX)
rows = math.ceil(img_height / A4_HEIGHT_PX)

# --- Création du PDF multipage ---
c = canvas.Canvas(output_pdf, pagesize=A4)

for row in range(rows):
    for col in range(cols):
        # Calcul du rectangle à découper
        left = col * A4_WIDTH_PX
        upper = row * A4_HEIGHT_PX
        right = min(left + A4_WIDTH_PX, img_width)
        lower = min(upper + A4_HEIGHT_PX, img_height)
        box = (left, upper, right, lower)
        tile = img.crop(box)

        # Nom temporaire du fichier
        tile_path = f"tile_{row}_{col}.png"
        tile.save(tile_path)

        # Calcul de la taille réelle de la tuile (pour la dernière colonne/ligne)
        tile_width = right - left
        tile_height = lower - upper
        # Conversion en points PDF (1 point = 1/72 inch)
        tile_width_pt = tile_width * 72 / dpi
        tile_height_pt = tile_height * 72 / dpi

        # Placement à l'origine de la page (0,0)
        c.drawImage(
            tile_path,
            0,
            A4[1] - tile_height_pt,  # Reportlab place l'origine en bas à gauche
            width=tile_width_pt,
            height=tile_height_pt
        )
        c.showPage()
        os.remove(tile_path)  # Nettoyage du fichier temporaire

c.save()
print(f"Exported poster to {output_pdf}")