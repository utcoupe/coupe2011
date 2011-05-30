import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.PrintWriter;
import java.io.Serializable;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.*;

import javax.imageio.ImageIO;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JSlider;
import javax.swing.JTextArea;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class Main {

	final static String TAG = "[Main] ";
	static Configuration configuration = new Configuration();
	static PictureSupplier pictureSupplier;

	static String projectPath = "";
	static String classPath = "";
	static String configPath = "main_config.data";
	static boolean enableView = true;

	static public void main(String[] args) {
		System.out.println(TAG + "starting...");
		
		// path
		URL str = Main.class.getProtectionDomain().getCodeSource().getLocation();
		classPath = str.getPath();
		projectPath = classPath + "..";
		System.setProperty("user.dir", projectPath);
		
		// set config
		if (args.length == 1) {
			if (args[0].equals("noview")) {
				enableView = false;
			}
		}
		

		load();
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				save();
			}
		});

		// make model
		pictureSupplier = new PictureSupplier();

		// connect clients
		NetworkClient.connectNetwork(pictureSupplier);
		if (enableView) {
			new ViewClientFrame(pictureSupplier);
		}

		System.out.println(TAG + "started");
	}

	static private void save() {
		if (configuration == null)
			return;

		FileOutputStream f_out;
		try {
			f_out = new FileOutputStream(projectPath + File.separator + configPath);
			ObjectOutputStream obj_out = new ObjectOutputStream(f_out);
			obj_out.writeObject(configuration);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@SuppressWarnings("unchecked")
	static private void load() {
		FileInputStream f_in;
		try {
			f_in = new FileInputStream(projectPath + File.separator + configPath);
			ObjectInputStream obj_in = new ObjectInputStream(f_in);
			Main.configuration = (Configuration) obj_in.readObject();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}
}

class Configuration implements Serializable {
	private static final long serialVersionUID = 1L;

	// hm
	int topIgnore = 0;

	// pawn
	int pawnPixelReference = 16570675;
	int pawnMagic = 10;

	// view
	boolean showVerticalLine = false;
	boolean showPawnsPixels = false;
	boolean showFigures = false;
	boolean showPawnsArrows = true;
}

class NetworkClient extends Thread implements Observer {
	Socket socket;
	PictureSupplier pictureSupplier;

	PrintWriter out;
	BufferedReader in;
	String idmsg;
	String idcmd;

	final static int PORT_REMOTE = 50000;
	final static int PORT_LOCAL = 1234;
	final static String IP_REMOTE = "192.168.1.136";
	final static String TAG = "[NetworkClient] ";

	static public void connectNetwork(final PictureSupplier pictureSupplier) {

		new Thread(new Runnable() {
			public void run() {
				try {
					System.out.println(TAG + "serveur remote port : " + PORT_REMOTE);
					Socket socketServeurPython = new Socket(IP_REMOTE, PORT_REMOTE);
					NetworkClient client = new NetworkClient(socketServeurPython, pictureSupplier);
					client.run();
					System.out.println(TAG + "serveur remote connected");
				} catch (UnknownHostException e) {
					e.printStackTrace();
				} catch (IOException e) {
					System.out.println(TAG + "serveur remote error");
				}
			}
		}).start();

		System.out.println(TAG + "serveur local port : " + PORT_LOCAL);
		final ServerSocket srvr;
		try {
			srvr = new ServerSocket(PORT_LOCAL);
			new Thread(new Runnable() {
				public void run() {
					NetworkClient client;
					try {
						System.out.println(TAG + "serveur local waiting");
						client = new NetworkClient(srvr.accept(), pictureSupplier);
						client.run();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			}).start();
			System.out.println(TAG + "serveur local connected");
		} catch (IOException e) {
			System.out.println(TAG + "serveur local erreur :");
		}
	}

	public NetworkClient(Socket socket, PictureSupplier pictureSupplier) {
		this.socket = socket;
		this.pictureSupplier = pictureSupplier;
	}

	public void run() {
		try {
			out = new PrintWriter(socket.getOutputStream(), true);
			in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

			while (true) {
				System.out.println("tcp client new loop");
				String line = in.readLine();
				System.out.println("line read = " + line);
				String[] args = line.split("\\.");
				if (args.length != 3) {
					out.println("bad input (" + args.length + "), usage: id.msg.cmd");
					continue;
				}
				idmsg = args[1];
				idcmd = args[2];
				if (idcmd.equals("0")) {
					// identification
					out.println("-1." + idmsg + ".5.balise");
				} else if (idcmd.equals("1")) {
					// ping
					out.println("-1.Pong");
				} else if (idcmd.equals("69")) {
					// exit
					// out.println("Pong");
				} else if (idcmd.equals("80")) { // figures
					// take photo
					pictureSupplier.addObserver(this);
					pictureSupplier.takePhoto();
				} else if (idcmd.equals("81")) {
					// take cache
					pictureSupplier.addObserver(this);
					pictureSupplier.takeCache();
				} else {
					out.println("bad input, usage: cmd = 71 || 72");
				}
			}

		} catch (IOException e) {
			System.out.println("erreur IO avec le socket port " + socket.getPort());
			e.printStackTrace();
		} catch (Exception e) {
			System.out.println("déconnexion client port " + socket.getPort());
			try {
				in.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			out.close();
		} finally {
			pictureSupplier.deleteObserver(this);
			try {
				System.out.println("fermeture socket... port " + socket.getPort());
				socket.close();
				System.out.println("socket fermé port " + socket.getPort());
			} catch (IOException e) {
				System.out.println("erreur de fermeture du socket port " + socket.getPort());
			}
		}
	}

	public void update(Observable arg0, Object arg1) {
		System.out.println("sending to tcp client...");
		pictureSupplier.deleteObserver(this);

		boolean[] figures = pictureSupplier.mFigureFinder.getFigures();
		out.println("-1." + idmsg + "." + "[" + (figures[0] ? "1" : "0") + "," + (figures[1] ? "1" : "0") + "," + (figures[2] ? "1" : "0") + "," + (figures[3] ? "1" : "0") + "," + (figures[4] ? "1" : "0") + "]");
		System.out.println("sending to tcp client...done");
	}
}

class PictureSupplier extends Observable implements Observer {
	BufferedImage image;
	public PawnFinder mPawnFinder;
	public FigureFinder mFigureFinder = new FigureFinder();
	public int width;
	public int height;

	final static String TAG = "[PictureSupplier] ";

	public PictureSupplier() {
		mPawnFinder = new PawnFinder();
		takeCache();
	}

	public void takeCache() {
		System.out.println(TAG + "take cache...");
		BufferedImage image = null;
		try {
			File file = new File(Main.projectPath + File.separator + "img/shot-utc-polytech.jpg");
			image = ImageIO.read(file);
			image = ImageUtils.rotate(image, 90);
			this.image = image;
			calculateAll();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void takePhoto() {
		System.out.println(TAG + "take photo...");
		BufferedImage image = null;
		try {
			//URL url = new URL("http://192.168.43.1:8080/shot.jpg");
			URL url = new URL("http://192.168.1.1:8080/shot.jpg");
			image = ImageIO.read(url);
			image = ImageUtils.rotate(image, 90);
			this.image = image;
			calculateAll();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void calculateAll() {
		System.out.println(TAG + "using new image");

		width = image.getWidth();
		height = image.getHeight();

		mPawnFinder.deleteObserver(this);
		mFigureFinder.deleteObserver(this);

		mPawnFinder.setImage(image);
		mFigureFinder.setImage(image);

		mPawnFinder.addObserver(this);
		mFigureFinder.addObserver(this);

		setChanged();
		notifyObservers(image);
	}

	public BufferedImage getImage() {
		return image;
	}

	public void update(Observable arg0, Object arg1) {
		setChanged();
		notifyObservers(image);
	}
}

class PawnFinder extends Observable {
	final static String TAG = "[PawnFinder] ";

	BufferedImage image;
	private boolean[][] mYellowPixelMatrix;
	int width;
	int height;
	ArrayList<Cluster> clusters;

	// benchmark
	public int nbPixelsFound;
	public int[] histogrammeH;
	public long timeLoop;
	public long timeClustering;

	public PawnFinder() {
		System.out.println(TAG + "()");
	}

	public void setImage(BufferedImage image) {
		this.image = image;
		calculate();
	}

	public void calculate() {
		System.out.println(TAG + "finding pawns...");
		long time1 = System.currentTimeMillis();

		histogrammeH = new int[360];

		int pixelReference = Main.configuration.pawnPixelReference;
		int magic = Main.configuration.pawnMagic;

		// d�tection des points jaunes
		width = image.getWidth();
		height = image.getHeight();
		boolean[][] yellowPixelMatrix = new boolean[width][height];
		int[] hsbvals = new int[3];
		int pixel, red, green, blue;
		nbPixelsFound = 0;
		for (int y = Main.configuration.topIgnore; y < height; y++) {
			for (int x = 0; x < width; x++) {
				pixel = image.getRGB(x, y);

				red = (pixel & 0xff0000) >> 16;
				green = (pixel & 0xff00) >> 8;
				blue = pixel & 0xff;

				if (Math.abs(red - ((pixelReference & 0xff0000) >> 16)) < magic && Math.abs(green - ((pixelReference & 0xff00) >> 8)) < magic && Math.abs(blue - ((pixelReference & 0xff))) < magic) {
					yellowPixelMatrix[x][y] = true;
					nbPixelsFound++;
				}
				// ImageUtils.rgb2hsl(red, green, blue, hsbvals);
				//
				// histogrammeH[hsbvals[0]]++;
				//
				// // #FCD933 == 16570675
				// if (40 < hsbvals[0] && hsbvals[0] < 70 && 50 < hsbvals[2] &&
				// 50 < hsbvals[1]) {
				// yellowPixelMatrix[x][y] = true;
				// nbPixelsFound++;
				// }
			}
		}
		mYellowPixelMatrix = yellowPixelMatrix;

		long time2 = System.currentTimeMillis();
		timeLoop = time2 - time1;

		clusters = Cluster.makeClusters(yellowPixelMatrix);
		long time3 = System.currentTimeMillis();
		timeClustering = time3 - time2;

		System.out.println(TAG + "done, pixels found = " + nbPixelsFound + ", clusters = " + clusters.size());

		setChanged();
		notifyObservers();
	}

	public ArrayList<Cluster> getClustersByMinSize(int sizeMin) {
		ArrayList<Cluster> _clusters = new ArrayList<Cluster>();
		for (Cluster cluster : clusters) {
			if (cluster.getPoints().size() > sizeMin) {
				_clusters.add(cluster);
			}
		}
		return _clusters;
	}

	public boolean[][] getYellowPixelMatrix() {
		boolean[][] yellowPixelMatrix = new boolean[width][height];
		for (int i = 0; i < width; i++) {
			System.arraycopy(mYellowPixelMatrix[i], 0, yellowPixelMatrix[i], 0, mYellowPixelMatrix.length);
			// for (int j = 0; j < height; j++)
			// yellowPixelMatrix[i][j] = mYellowPixelMatrix[i][j];
		}
		return yellowPixelMatrix;
	}

	public void setMagic(int value) {
		magic = value;
		calculate();
	}

	int magic;
}

@SuppressWarnings("serial")
class FigureFinder extends Observable implements java.io.Serializable, Observer {
	BufferedImage image;

	Rectangle[] figureZones = new Rectangle[5];
	boolean[] figures = new boolean[5];
	float[] figuresPc = new float[5];
	int figureValueS;
	int figureValueL;
	int figurePourcent;

	final static String TAG = "[FigureFinder] ";

	public FigureFinder() {
		setZoneFigures(new Rectangle(0, 0, 200, 50));
		load();
	}

	public int getFigureValueS() {
		return figureValueS;
	}

	public void setFigureValueS(int figureValueS) {
		this.figureValueS = figureValueS;
		calculate();
	}

	public int getFigureValueL() {
		return figureValueL;
	}

	public void setFigureValueL(int figureValueL) {
		this.figureValueL = figureValueL;
		calculate();
	}

	public Rectangle[] getFigureZones() {
		return figureZones;
	}

	public void setImage(BufferedImage image) {
		this.image = image;
		calculate();
	}

	private void calculate() {
		if (this.image == null)
			return;

		float total = 0f;
		System.out.println(TAG + "calculating...");
		for (int i = 0; i < 5; i++) {
			figuresPc[i] = getRateFigureDetected(figureZones[i]);
			total += figuresPc[i];
		}
		float moyenne = total / 5;
		for (int i = 0; i < 5; i++) {
			if (figuresPc[i] > moyenne) {
				// beaucoup jaune => figure
				figures[i] = true;
			} else {
				// sinon table (pion)
				figures[i] = false;
			}
		}

		System.out.println(TAG + "work done, notify");
		setChanged();
		notifyObservers();
	}

	public void setZoneFigures(Rectangle rect) {
		double width = rect.getWidth() / 5;
		figureZones[0] = (new Rectangle((int) (rect.getX() + width * 0), (int) rect.getY(), (int) width - 1, (int) rect.getHeight()));
		figureZones[1] = (new Rectangle((int) (rect.getX() + width * 1), (int) rect.getY(), (int) width - 1, (int) rect.getHeight()));
		figureZones[2] = (new Rectangle((int) (rect.getX() + width * 2), (int) rect.getY(), (int) width - 1, (int) rect.getHeight()));
		figureZones[3] = (new Rectangle((int) (rect.getX() + width * 3), (int) rect.getY(), (int) width - 1, (int) rect.getHeight()));
		figureZones[4] = (new Rectangle((int) (rect.getX() + width * 4), (int) rect.getY(), (int) width - 1, (int) rect.getHeight()));

		calculate();
	}

	public void save() {
		FileOutputStream f_out;
		try {
			f_out = new FileOutputStream(Main.projectPath + File.separator + "PictureModel.data");
			ObjectOutputStream obj_out = new ObjectOutputStream(f_out);

			obj_out.writeInt(figureValueS);
			obj_out.writeInt(figureValueL);
			obj_out.writeObject(figureZones);
			obj_out.writeObject(figures);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void load() {
		FileInputStream f_in;
		try {
			f_in = new FileInputStream(Main.projectPath + File.separator + "PictureModel.data");
			ObjectInputStream obj_in = new ObjectInputStream(f_in);

			figureValueS = (int) obj_in.readInt();
			figureValueL = (int) obj_in.readInt();
			figureZones = (Rectangle[]) obj_in.readObject();
			figures = (boolean[]) obj_in.readObject();

		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}

	private float getRateFigureDetected(Rectangle zone) {
		Rectangle imageRectangle = new Rectangle(image.getWidth(), image.getHeight());
		zone = imageRectangle.intersection(zone);

		float pixelsOthers = 0, pixelsFigure = 1;
		int x1 = (int) zone.getX();
		int y1 = (int) zone.getY();
		int x2 = (int) zone.getWidth() + x1;
		int y2 = (int) zone.getHeight() + y1;

		for (int y = y1; y < y2; y++) {
			for (int x = x1; x < x2; x++) {
				int pixel = image.getRGB(x, y);

				int[] hsbvals = new int[3];
				int red = (pixel & 0xff0000) >> 16;
				int green = (pixel & 0xff00) >> 8;
				int blue = pixel & 0xff;
				ImageUtils.rgb2hsl(red, green, blue, hsbvals);

				// |+++++++++++++++++++++[figureValueL]---------------|
				// if (hsbvals[1] < figureValueS && hsbvals[2] < figureValueL)
				// pixelsTableBorder++;
				// else
				// pixelsOthers++;
				// boolean teinteJaune = 30 < hsbvals[0] && hsbvals[0] < 60;
				boolean jaune = figureValueS * 255 / 100 < red && figureValueS * 255 / 100 < green;
				// boolean seuil = hsbvals[1] > figureValueS;
				if (jaune)
					pixelsFigure++;
				else
					pixelsOthers++;
			}
		}
		return (pixelsFigure / (pixelsFigure + pixelsOthers) * 100f);
	}

	public boolean[] getFigures() {
		return figures;
	}

	public void update(Observable arg0, Object arg1) {
		BufferedImage image = (BufferedImage) arg1;
		setImage(image);
	}
}

class Histogramme extends JPanel {
	private static final long serialVersionUID = 1L;

	int[] data;

	public Histogramme() {
		setPreferredSize(new Dimension(180, 40));
	}

	public void setData(int[] data) {
		this.data = data;
		repaint();
	}

	public void paint(Graphics g) {
		if (data != null) {
			g.setColor(Color.white);
			g.fillRect(0, 0, getWidth(), getHeight());
			float max = 1;
			for (int x = 0; x < data.length; x++) {
				if (max < data[x])
					max = data[x];
			}
			for (int x = 0; x < data.length; x++) {
				g.setColor(Color.getHSBColor(x / 360f, 1, 1));
				g.drawLine((int) (x / 360f * 180f), 0, (int) (x / 360f * 180f), (int) (data[x] / max * 40.0));
			}
		}
	}
}

@SuppressWarnings("serial")
class ViewClientFrame extends JFrame implements ActionListener, ChangeListener, Observer {

	JButton mJButtonPhoto = new JButton("photo");
	JButton mJButtonCache = new JButton("cache");
	JButton mJButtonMagicPixel = new JButton("pixel magique");

	JTextArea mJTextAreaTime = new JTextArea();

	JSlider mJSliderH = new JSlider(0, 360);
	JSlider mJSliderS = new JSlider(0, 100);
	JSlider mJSliderL = new JSlider(0, 100);
	JSlider mJSliderMagic = new JSlider(0, 255);
	JSlider mJSliderTop = new JSlider(0, 400);

	JCheckBox mJCheckBoxVerticalLine = new JCheckBox();
	JCheckBox mJCheckBoxShowPixels = new JCheckBox();
	JCheckBox mJCheckBoxPawnsArrows = new JCheckBox();
	JCheckBox mJCheckBoxFigures = new JCheckBox();

	JPanel mPanelTop = new JPanel();
	JPanel mPanelRight = new JPanel();

	JPanel mJPanelMagicPixel = new JPanel();
	Histogramme mHistogramme = new Histogramme();

	PictureSupplier mPictureSupplier;
	FigureFinder figureFinder;
	Manager mManager;

	final static String TAG = "[ViewClientFrame] ";

	public ViewClientFrame(final PictureSupplier mPictureSupplier) {
		this(mPictureSupplier, mPictureSupplier.mFigureFinder);
		System.out.println(TAG + "constructeur");
	}

	public ViewClientFrame(final PictureSupplier mPictureSupplier, final FigureFinder mFigureFinder) {
		this.mPictureSupplier = mPictureSupplier;
		this.figureFinder = mFigureFinder;
		mPictureSupplier.addObserver(this);

		mJSliderH.disable();

		// set values
		mJSliderS.setValue(figureFinder.getFigureValueS());
		mJSliderL.setValue(figureFinder.getFigureValueL());
		mJSliderMagic.setValue(Main.configuration.pawnMagic);
		mJSliderTop.setValue(Main.configuration.topIgnore);
		mJCheckBoxVerticalLine.setSelected(Main.configuration.showVerticalLine);
		mJCheckBoxShowPixels.setSelected(Main.configuration.showPawnsPixels);
		mJCheckBoxPawnsArrows.setSelected(Main.configuration.showPawnsArrows);
		mJCheckBoxFigures.setSelected(Main.configuration.showFigures);

		// listeners
		mJButtonPhoto.addActionListener(this);
		mJButtonCache.addActionListener(this);
		mJButtonMagicPixel.addActionListener(this);
		mJSliderH.addChangeListener(this);
		mJSliderS.addChangeListener(this);
		mJSliderL.addChangeListener(this);
		mJSliderMagic.addChangeListener(this);
		mJSliderTop.addChangeListener(this);
		mJCheckBoxVerticalLine.addChangeListener(this);
		mJCheckBoxShowPixels.addChangeListener(this);
		mJCheckBoxPawnsArrows.addChangeListener(this);
		mJCheckBoxFigures.addChangeListener(this);

		mPanelRight.setPreferredSize(new Dimension(200, 800));
		mPanelRight.setLayout(new FlowLayout());

		// add right
		mPanelRight.add(new JLabel("Top bordure:"));
		mPanelRight.add(mJSliderTop);
		mPanelRight.add(new JLabel("HSL figures/bordure:"));
		mPanelRight.add(mJSliderH);
		mPanelRight.add(mJSliderS);
		mPanelRight.add(mJSliderL);
		mPanelRight.add(new JLabel("Baguette magique:"));
		mPanelRight.add(mJSliderMagic);
		mPanelRight.add(mJButtonMagicPixel);
		mPanelRight.add(new JLabel("Ligne verticale:"));
		mPanelRight.add(mJCheckBoxVerticalLine);
		mPanelRight.add(new JLabel("Afficher pixels:"));
		mPanelRight.add(mJCheckBoxShowPixels);
		mPanelRight.add(new JLabel("Fleches pions:"));
		mPanelRight.add(mJCheckBoxPawnsArrows);
		mPanelRight.add(new JLabel("Figures:"));
		mPanelRight.add(mJCheckBoxFigures);
		mPanelRight.add(mPanelTop);
		mPanelRight.add(mJTextAreaTime);
		mPanelRight.add(mHistogramme);
		mPanelRight.add(mJPanelMagicPixel);
		mPanelRight.setSize(200, 200);

		mJPanelMagicPixel.setPreferredSize(new Dimension(180, 40));

		mPanelTop.add(mJButtonPhoto);
		mPanelTop.add(mJButtonCache);

		mManager = new Manager(mPictureSupplier, this);
		mManager.setSize(mPictureSupplier.width, mPictureSupplier.height);

		this.setLayout(new BorderLayout());
		this.add(mManager);
		this.add(mPanelRight, BorderLayout.EAST);

		this.addWindowListener(new java.awt.event.WindowAdapter() {
			public void windowClosing(WindowEvent winEvt) {
				mFigureFinder.save();
			}
		});

		update(null, null);

		setTitle("Configuration");
		setVisible(true);
		setResizable(false);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		pack();
	}

	public void actionPerformed(ActionEvent e) {
		System.out.println("button pressed");
		if (e.getSource() == mJButtonPhoto) {
			mPictureSupplier.takePhoto();
		} else if (e.getSource() == mJButtonCache) {
			mPictureSupplier.takeCache();
		} else if (e.getSource() == mJButtonMagicPixel) {
			mManager.takeMagicPixel();
		} else {
			System.out.println("unexpected button");
		}
	}

	public void stateChanged(ChangeEvent e) {
		if (e.getSource() == mJSliderS) {
			int valueS = mJSliderS.getValue();
			figureFinder.setFigureValueS(valueS);
		} else if (e.getSource() == mJSliderL) {
			int valueL = mJSliderL.getValue();
			figureFinder.setFigureValueL(valueL);
		} else if (e.getSource() == mJCheckBoxVerticalLine) {
			Main.configuration.showVerticalLine = mJCheckBoxVerticalLine.isSelected();
			mManager.repaint();
		} else if (e.getSource() == mJCheckBoxShowPixels) {
			Main.configuration.showPawnsPixels = mJCheckBoxShowPixels.isSelected();
			mManager.repaint();
		} else if (e.getSource() == mJCheckBoxPawnsArrows) {
			Main.configuration.showPawnsArrows = mJCheckBoxPawnsArrows.isSelected();
			mManager.repaint();
		} else if (e.getSource() == mJCheckBoxFigures) {
			Main.configuration.showFigures = mJCheckBoxFigures.isSelected();
			mManager.repaint();
		} else if (e.getSource() == mJSliderMagic) {
			mPictureSupplier.mPawnFinder.setMagic(mJSliderMagic.getValue());
			Main.configuration.pawnMagic = mJSliderMagic.getValue();
		} else if (e.getSource() == mJSliderTop) {
			Main.configuration.topIgnore = mJSliderTop.getValue();
			Main.pictureSupplier.calculateAll();
		}
	}

	public void update(Observable arg0, Object arg1) {
		mJSliderS.setValue(figureFinder.getFigureValueS());
		mJSliderL.setValue(figureFinder.getFigureValueL());
		mJTextAreaTime.setText("loop: " + mPictureSupplier.mPawnFinder.timeLoop + "ms\nclustering: " + mPictureSupplier.mPawnFinder.timeClustering + "ms");
		mHistogramme.setData(mPictureSupplier.mPawnFinder.histogrammeH);
		mJPanelMagicPixel.setBackground(new Color(Main.configuration.pawnPixelReference));
	}
}

@SuppressWarnings("serial")
class Manager extends JPanel implements MouseListener, Observer {

	final static String TAG = "[Manager] ";

	PictureSupplier mPictureSupplier;
	FigureFinder mFigureFinder;
	ViewClientFrame viewClientFrame;

	int x1, y1;
	int x2, y2;
	boolean takingMagicPixel;

	public Manager(PictureSupplier mPictureSupplier, ViewClientFrame viewClientFrame) {
		this.mPictureSupplier = mPictureSupplier;
		this.mFigureFinder = mPictureSupplier.mFigureFinder;
		this.viewClientFrame = viewClientFrame;
		mPictureSupplier.addObserver(this);

		setPreferredSize(new Dimension(580, 800));
		addMouseListener(this);
	}

	public void takeMagicPixel() {
		takingMagicPixel = true;
	}

	public void paintComponent(Graphics g) {
		System.out.println(TAG + "paintComponent");

		// reset image
		g.setColor(Color.white);
		g.fillRect(0, 0, 2000, 2000);

		// draw image
		g.drawImage(mPictureSupplier.getImage(), 0, 0, null);

		// draw top
		g.setColor(new Color(0, 0, 0, 150));
		g.fillRect(0, 0, mPictureSupplier.getImage().getWidth(), Main.configuration.topIgnore);

		// ligne verticale centr�e
		if (viewClientFrame.mJCheckBoxVerticalLine.isSelected()) {
			g.drawLine(mPictureSupplier.width / 2, 0, mPictureSupplier.width / 2, getHeight());
		}

		// draw figure
		if (Main.configuration.showFigures) {
			boolean[] figures = mFigureFinder.getFigures();
			Rectangle[] zones = mFigureFinder.getFigureZones();

			BufferedImage imagePion = null;
			BufferedImage imageFigure = null;
			try {
				imagePion = ImageIO.read(new File(Main.projectPath + File.separator + "img/pion.png"));
				imageFigure = ImageIO.read(new File(Main.projectPath + File.separator + "img/figure.png"));
			} catch (IOException e) {
				e.printStackTrace();
			}

			for (int i = 0; i < 5; i++) {
				if (figures[i]) {
					g.setColor(Color.WHITE);
					// g.drawString("Figure", zones[i].x, zones[i].y);
					g.drawImage(imageFigure, zones[i].x - imageFigure.getWidth() / 2 + zones[i].width / 2, zones[i].y - imageFigure.getHeight() - 10, new Color(0, 0, 0, 1), null);
					g.setColor(Color.YELLOW);
				} else {
					g.setColor(Color.WHITE);
					// g.drawString("Pion", (int) zones[i].getX(), (int)
					// zones[i].getY());
					g.drawImage(imagePion, zones[i].x - imagePion.getWidth() / 2 + zones[i].width / 2, zones[i].y - imagePion.getHeight() - 10, new Color(0, 0, 0, 1), null);
					g.setColor(Color.BLACK);
				}
				drawRect(g, zones[i]);
			}
		}

		// d�tection des points jaunes
		ArrayList<Cluster> clusters = mPictureSupplier.mPawnFinder.getClustersByMinSize(200);

		// affiche les clusters
		boolean alt = true;

		BufferedImage imageArrow = null;
		try {
			imageArrow = ImageIO.read(new File(Main.projectPath + File.separator + "img/arrow.png"));
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println(TAG + "clusters, nb clusters : " + clusters.size());
		for (Cluster cluster : clusters) {
			// pour chaque cluster assez grand
			if (cluster.getPoints().size() > 100) {
				if (alt)
					g.setColor(Color.BLUE);
				else
					g.setColor(Color.CYAN);

				if (Main.configuration.showPawnsPixels) {
					for (Point point : cluster.getPoints()) {
						g.drawLine(point.x, point.y, point.x, point.y);
					}
				}

				if (Main.configuration.showPawnsArrows) {
					g.drawImage(imageArrow, cluster.center.x - imageArrow.getWidth() / 2, cluster.center.y - imageArrow.getHeight(), new Color(0, 0, 0, 1), null);
					g.setColor(Color.YELLOW);

					textbn(g, cluster.reelLocation.x + "," + cluster.reelLocation.y, cluster.center.x + 15, cluster.center.y - 15);
					textbn(g, pixel2mm(cluster.reelLocation.y) + "mm", cluster.center.x + 15, cluster.center.y);
				}
			}
			alt = !alt;
		}
	}

	private void textbn(Graphics g, String str, int x, int y) {
		char[] text = str.toCharArray();
		g.setColor(Color.black);
		g.drawChars(text, 0, text.length, x + 1, y - 0);
		g.setColor(Color.white);
		g.drawChars(text, 0, text.length, x + 0, y - 1);
	}

	public float pixel2mm(int y) {
		if (y > 396) {
			// = -0,027x2 + 21,82x - 2236
			return (float) (-0.027 * Math.pow((double) y, (double) 2) + 21.82 * y - 2236);
		} else {
			// = -1,519x + 2801
			return (float) (-1.519 * y + 2801);
		}

		// = 0,009x2 + 2,631x + 2775
		// = -0,027x2 + 21,82x - 2236
		// return (float) (-0.009 * Math.pow((double)y, (double)2) + 2.631 * y +
		// 2775);
		// return (float) (403.6 * Math.pow((double)pixels, (double)0.300));
	}

	public void drawRect(Graphics g, Rectangle zone) {
		int x1 = (int) zone.getX();
		int y1 = (int) zone.getY();
		int x2 = (int) zone.getWidth() + x1;
		int y2 = (int) zone.getHeight() + y1;

		g.fillRect(x1, y1, x2 - x1, y2 - y1);
	}

	public void mouseClicked(MouseEvent e) {
	}

	public void mouseEntered(MouseEvent e) {
	}

	public void mouseExited(MouseEvent e) {
	}

	public void mousePressed(MouseEvent e) {
		System.out.println(TAG + "mousePressed");
		if (takingMagicPixel) {
			int rgb = mPictureSupplier.getImage().getRGB(e.getX(), e.getY());
			Main.configuration.pawnPixelReference = rgb;
			Main.pictureSupplier.calculateAll();
		} else {
			x1 = e.getX();
			y1 = e.getY();
		}
	}

	public void mouseReleased(MouseEvent e) {
		System.out.println(TAG + "mouseReleased");
		if (takingMagicPixel) {
			takingMagicPixel = false;
		} else {
			x2 = e.getX();
			y2 = e.getY();

			mFigureFinder.setZoneFigures(new Rectangle(x1, y1, x2 - x1, y2 - y1));

			repaint();
		}
	}

	public void update(Observable arg0, Object arg1) {
		System.out.println(TAG + "view got an update");
		repaint();
	}
}
