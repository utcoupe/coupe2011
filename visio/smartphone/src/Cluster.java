import java.awt.Point;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Stack;

public class Cluster {
	private ArrayList<Point> points = new ArrayList<Point>();
	public Point top = new Point(10000, 10000);
	public Point right = new Point(0, 0);
	public Point bottom = new Point(0, 0);
	public Point left = new Point(10000, 10000);
	public Point lineLeft = new Point();
	public Point lineRight = new Point();
	public Point lineTop = new Point();
	public Point lineBottom = new Point();
	public Point center;

	// x = 0mm center
	// y = 0mm bottom
	public Point reelLocation;

	// image dimension
	public int width;
	public int height;

	public ArrayList<Point> getPoints() {
		return points;
	}

	public void setPoints(ArrayList<Point> points) {
		this.points = points;
	}

	public Cluster() {

	}

	static public ArrayList<Cluster> makeClusters(boolean[][] pixelMatrix) {
		// declaration
		int width = pixelMatrix.length;
		int height = pixelMatrix[0].length;
		ArrayList<Cluster> clusters = new ArrayList<Cluster>();
		int xCurrentPoint, yCurrentPoint;

		// make clusters
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (pixelMatrix[x][y]) {
					Cluster cluster = new Cluster();
					cluster.height = height;
					cluster.width = width;
					clusters.add(cluster);

					Stack<Point> stack = new Stack<Point>();
					stack.push(new Point(x, y));
					pixelMatrix[x][y] = false;
					cluster.center = new Point(x, y);

					HashMap<Integer, ArrayList<Point>> horizontalLine = new HashMap<Integer, ArrayList<Point>>();
					HashMap<Integer, ArrayList<Point>> verticalLine = new HashMap<Integer, ArrayList<Point>>();

					while (!stack.isEmpty()) {
						// get point
						Point currentPoint = stack.pop();
						cluster.points.add(currentPoint);
						xCurrentPoint = currentPoint.x;
						yCurrentPoint = currentPoint.y;

						cluster.center.x += xCurrentPoint;
						cluster.center.y += yCurrentPoint;

						// points by vertical line
						if (horizontalLine.containsKey(yCurrentPoint)) {
							horizontalLine.get(yCurrentPoint).add(currentPoint);
						} else {
							ArrayList<Point> line = new ArrayList<Point>();
							line.add(currentPoint);
							horizontalLine.put(yCurrentPoint, line);
						}

						// points by horizontal line
						if (verticalLine.containsKey(xCurrentPoint)) {
							verticalLine.get(xCurrentPoint).add(currentPoint);
						} else {
							ArrayList<Point> line = new ArrayList<Point>();
							line.add(currentPoint);
							verticalLine.put(xCurrentPoint, line);
						}

						// save extra points
						if (xCurrentPoint < cluster.left.x) {
							cluster.left.x = xCurrentPoint;
							cluster.left.y = yCurrentPoint;
						}
						if (cluster.right.x < xCurrentPoint) {
							cluster.right.x = xCurrentPoint;
							cluster.right.y = yCurrentPoint;
						}
						if (yCurrentPoint < cluster.top.y) {
							cluster.top.x = xCurrentPoint;
							cluster.top.y = yCurrentPoint;
						}
						if (cluster.bottom.y < yCurrentPoint) {
							cluster.bottom.x = xCurrentPoint;
							cluster.bottom.y = yCurrentPoint;
						}

						// find neighbors
						if (0 <= xCurrentPoint - 1 && pixelMatrix[xCurrentPoint - 1][yCurrentPoint]) {
							pixelMatrix[xCurrentPoint - 1][yCurrentPoint] = false;
							stack.push(new Point(xCurrentPoint - 1, yCurrentPoint));
						}
						if (xCurrentPoint + 1 < width && pixelMatrix[xCurrentPoint + 1][yCurrentPoint]) {
							pixelMatrix[xCurrentPoint + 1][yCurrentPoint] = false;
							stack.push(new Point(xCurrentPoint + 1, yCurrentPoint));
						}
						if (0 <= yCurrentPoint - 1 && pixelMatrix[xCurrentPoint][yCurrentPoint - 1]) {
							pixelMatrix[xCurrentPoint][yCurrentPoint - 1] = false;
							stack.push(new Point(xCurrentPoint, yCurrentPoint - 1));
						}
						if (yCurrentPoint + 1 < height && pixelMatrix[xCurrentPoint][yCurrentPoint + 1]) {
							pixelMatrix[xCurrentPoint][yCurrentPoint + 1] = false;
							stack.push(new Point(xCurrentPoint, yCurrentPoint + 1));
						}
					} // end while

					cluster.computeHorizontalLine(horizontalLine);
					cluster.computeVerticalLine(verticalLine);

					// center
					cluster.computeCenter();

					// reel location
					cluster.computePixLocationFromBottom();

				} // end cluster
			} // end for
		} // end for

		// return clusters
		return clusters;
	}

	private void computeHorizontalLine(HashMap<Integer, ArrayList<Point>> horizontalLine) {
		// find largest line
		int maxHorizontalPixels = 0;
		ArrayList<Point> largestHorizontalLine = null;
		for (ArrayList<Point> line : horizontalLine.values()) {
			if (maxHorizontalPixels < line.size()) {
				maxHorizontalPixels = line.size();
				largestHorizontalLine = line;
			}
		}

		// get extremes
		this.lineLeft.x = 100000;
		this.lineRight.x = 0;
		for (Point point : largestHorizontalLine) {
			if (point.x < this.lineLeft.x) {
				this.lineLeft = point;
			}
			if (this.lineRight.x < point.x) {
				this.lineRight = point;
			}
		}
	}

	private void computeVerticalLine(HashMap<Integer, ArrayList<Point>> verticalLine) {
		// find largest line
		int maxVerticalPixels = 0;
		ArrayList<Point> largestVerticalLine = null;
		for (ArrayList<Point> line : verticalLine.values()) {
			if (maxVerticalPixels < line.size()) {
				maxVerticalPixels = line.size();
				largestVerticalLine = line;
			}
		}

		// get extremes
		this.lineTop.y = 100000;
		this.lineBottom.y = 0;
		for (Point point : largestVerticalLine) {
			if (point.y < this.lineTop.y) {
				this.lineTop = point;
			}
			if (this.lineBottom.y < point.y) {
				this.lineBottom = point;
			}
		}
	}

	private void computeCenter() {
		// positions by average
		this.center.x = this.center.x / this.getPoints().size();
		this.center.y = this.center.y / this.getPoints().size();

		// correct center when cut by image border
		if (this.left.x == 0) {
			this.center.x = this.lineTop.x;
		} else if (this.right.x == width - 1) {
			this.center.x = this.lineTop.x;
		}
	}

	private void computePixLocationFromBottom() {
		this.reelLocation = new Point(this.center.x - width / 2, height - this.center.y);
	}
}