import java.awt.Graphics2D;
import java.awt.image.BufferedImage;

class ImageUtils {

	static public void getRGB(int color, int[] rgb)
	{
		
	}
	
	static public boolean[][] dilatation(boolean[][] pixelMatrix, boolean whichToDilate) {
		boolean[][] pixelMatrixDilat = new boolean[pixelMatrix.length][pixelMatrix[0].length];
		for (int y = 0; y < pixelMatrix[0].length; y++) {
			for (int x = 0; x < pixelMatrix.length; x++) {
				pixelMatrixDilat[x][y] = !whichToDilate;
			}
		}
		for (int y = 0; y < pixelMatrix[0].length; y++) {
			for (int x = 0; x < pixelMatrix.length; x++) {
				try {
					if (pixelMatrix[x][y] == whichToDilate) {
						pixelMatrixDilat[x][y] = whichToDilate;
						pixelMatrixDilat[x + 1][y] = whichToDilate;
						pixelMatrixDilat[x - 1][y] = whichToDilate;
						pixelMatrixDilat[x][y + 1] = whichToDilate;
						pixelMatrixDilat[x][y - 1] = whichToDilate;
					}
				} catch (Exception e) {

				}
			}
		}
		return pixelMatrixDilat;
	}
	
	static public void rgb2hsl(int r, int g, int b, int hsl[]) {

		float var_R = (r / 255f);
		float var_G = (g / 255f);
		float var_B = (b / 255f);

		float var_Min; // Min. value of RGB
		float var_Max; // Max. value of RGB
		float del_Max; // Delta RGB value

		if (var_R > var_G) {
			var_Min = var_G;
			var_Max = var_R;
		} else {
			var_Min = var_R;
			var_Max = var_G;
		}

		if (var_B > var_Max)
			var_Max = var_B;
		if (var_B < var_Min)
			var_Min = var_B;

		del_Max = var_Max - var_Min;

		float H = 0, S, L;
		L = (var_Max + var_Min) / 2f;

		if (del_Max == 0) {
			H = 0;
			S = 0;
		} // gray
		else { // Chroma
			if (L < 0.5)
				S = del_Max / (var_Max + var_Min);
			else
				S = del_Max / (2 - var_Max - var_Min);

			float del_R = (((var_Max - var_R) / 6f) + (del_Max / 2f)) / del_Max;
			float del_G = (((var_Max - var_G) / 6f) + (del_Max / 2f)) / del_Max;
			float del_B = (((var_Max - var_B) / 6f) + (del_Max / 2f)) / del_Max;

			if (var_R == var_Max)
				H = del_B - del_G;
			else if (var_G == var_Max)
				H = (1 / 3f) + del_R - del_B;
			else if (var_B == var_Max)
				H = (2 / 3f) + del_G - del_R;
			if (H < 0)
				H += 1;
			if (H > 1)
				H -= 1;
		}
		hsl[0] = (int) (360 * H);
		hsl[1] = (int) (S * 100);
		hsl[2] = (int) (L * 100);
	}

	public static BufferedImage rotate(BufferedImage img, int angle) {
		int w = img.getWidth();
		int h = img.getHeight();
		BufferedImage dimg = dimg = new BufferedImage(h, w, img.getType());
		Graphics2D g = dimg.createGraphics();
		g.rotate(Math.toRadians(angle), 0, 0);
		g.translate(0, -h);
		g.drawImage(img, null, 0, 0);
		return dimg;
	}
}