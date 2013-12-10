import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.ArrayList;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SwingWorker;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import java.applet.*;
import java.awt.*;
import javax.swing.*;


public class EvasionView extends JApplet implements EvasionListener {
  Object lock = new Object();
  class GPanel extends JPanel {
    /**
     * 
     */
    private static final long serialVersionUID = 4626419896777520312L;
    Graphics2D g2d;
    int height, width;
    public GPanel(int height, int width) {
      this.height = height;
      this.width = width;
      setPreferredSize(new Dimension(this.height, this.widht));
    }

    public GPanel() {
      this(400, 400);
    }

    @Override
    public void paintComponent(Graphics g) {
      super.paintComponent(g);
      g2d = (Graphics2D)g;
      g2d.setBackground(Color.WHITE);
      g2d.clearRect(0, 0, this.height, this.width);
      g2d.setColor(Color.GREEN);
      g2d.setStroke(new BasicStroke(1f));
      synchronized (lock) {
        for (Line2D wall : walls) {
          g2d.draw(wall);
        }
      }
      g2d.setColor(Color.BLACK);
      g2d.draw(new Line2D.Float(0,this.width,this.height,this.width));
      g2d.draw(new Line2D.Float(0,0, this.height, 0));
      g2d.draw(new Line2D.Float(0,0,0, this.width));
      g2d.draw(new Line2D.Float(this.width, 0, this.height, this.width));
      if (prePosition != null) {
        g2d.setColor(Color.BLUE);
        g2d.setStroke(new BasicStroke(3f,BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        g2d.draw(new Line2D.Float(preyPosition,preyPosition));
      }
      if (hunterPosition != null) {
        g2d.setColor(Color.RED);
        g2d.setStroke(new BasicStroke(5f));
        g2d.draw(new Line2D.Float(hunterPosition,hunterPosition));
      }
    }
  }
  final EvasionModel model = null;
  String name;
  long move_counter;
  long time_counter;

  int moves_to_next_wall;
  Point2D preyPosition;
  Point2D hunterPosition;
  HunterMoves hunterDirection;
  ArrayList<Line2D> walls = new ArrayList<Line2D>();
  JFrame frame;
  GPanel mainPanel = new GPanel();
  JTextArea gameDescription;
  JButton hunterButton;

  /* attributes needed by the game */
  final static int HUMAN = 0;
  final static int COMPUTER = 1;
  final int N = 3;
  final int W = 3;
  int hunter = COMPUTER;
  int prey = COMPUTER;

  String hunterName;
  String preyName;
  
  /* GUI components */
  JLabel displayString = new JLabel("DisplayLabel");
  JLabel hunterLabel = new JLabel("HunterLabel");
  JLabel preyLabel = new JLabel("PreyLabel");

  String[] nstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox NList = new JComboBox(nstrings);
  NList.setSelectedIndex(0);
  NList.addActionListener(new ActionListener {
    public void actionPerformed(ActionEvent e) {
          JComboBox cb = (JComboBox)e.getSource();
          String str = (String)cb.getSelectedItem();
          N = Integer.parseInt(str);
      }
  }
  );

  String[] wstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox WList = new JComboBox(wstrings);
  WList.setSelectedIndex(1);
  WList.addActionListener(new ActionListener {
    public void actionPerformed(ActionEvent e) {
          JComboBox cb = (JComboBox)e.getSource();
          String str = (String)cb.getSelectedItem();
          W = Integer.parseInt(str);
      }
  }
  );
 
  class RadioActionListener implements ActionListener {
    int id = 0;
    publid RadioActionListener(int id) {
      this.id = id;
    }
    public void actionPerformed(ActoinEvent e) {
      if(this.id == 0) {// hunter
        if ("Computer".equals(e.getActionCommand())) {
          hunter = COMPUTER;
        }else {
          hunter = HUMAN;
        }
      }
      else {
        if ("Computer".equals(e.getActionCommand())) {
          prey = COMPUTER;
        }else {
          prey = HUMAN;
        }
      }
    }
  }
  ButtonGroup hunterBG = new ButtonGroup();
  JRadioButton hunterComputer = new JRatioButton("Computer");
  JRadioButton hunterHuman = new JRatioButton("Human");
  hunterBG.add(hunterComputer);
  hunterBG.add(hunterHuman);
  hunterComputer.setSelected(true);
  hunterComputer.addActionListener(new RadioActionListener(0));
  hunterHuman.addActionListener(new RadioActionListener(0));
  
  ButtonGroup preyBG = new ButtonGroup();
  JRadioButton preyComputer = new JRatioButton("Computer");
  JRadioButton preyHuman = new JRatioButton("Human");
  preyBG.add(preyComputer);
  preyBG.add(preyHuman);
  preyComputer.setSelected(true);
  preyComputer.addActionListener(new RadioActionListener(1));
  preyHuman.addActionListener(new RadioActionListener(1));

  JButton startButton = new JButton("Start");
  JButton resetButton = new JButton("Reset");

  resetButton.addActionListener( new ActionLister() {
      public void actionPerformed(ActionEvent e) {
          walls = ArrayList<Line2D>();
          hunter = COMPUTER;
          prey = COMPUTER;
          N = 3;
          W = 3;
      }
    }
  });

  public void init() {
    Container container = getContentPane();
    container.setBackground(Color.WHITE);
    container.setLayout(new BorderLayout());
    container.add(displayString, BorderLayout.PAGE_START);
    
    JPanel gamePanel = new JPanel();
    container.add(gamePanel, BorderLayout.CENTER);
    
    gamePanel.setLayout(new FlowLayout());
    gamePanel.add(mainPanel);
    JPanel controlPanel = new JPanel();
    gamePanel.add(controlPanel);
    controlPanel.setLayout(new BoxLayout(controlPanel, BoxLayout.Y_AXIS));
    controlPanel.add(NList);
    controlPanel.add(WList);
    controlPanel.add(hunterLabel);
    controlPanel.add(hunterBG);
    controlPanel.add(preyLabel);
    controlPanel.add(preyBG);
    controlPanel.add(startButton);
    controlPanel.add(resetButton);
  }

  public void _init_container(Container frameContainer) {
    preyPosition = new Point(330,200);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    time_counter = 0;
    move_counter = 0;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();

    UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
    gameDescription = new JTextArea();
    mainPanel = new GPanel();
    mainPanel.setSize(500, 500);
    mainPanel.setBackground(Color.WHITE);
    //    mainPanel.setBorder(BorderFactory.createLineBorder(Color.black, 1));
    mainPanel.setVisible(true);
    frameContainer.add(mainPanel,BorderLayout.CENTER);
    frameContainer.add(gameDescription,BorderLayout.NORTH);
  }
  /*
  public EvasionView(EvasionModel model, int N, int W) throws ClassNotFoundException, InstantiationException, IllegalAccessException, UnsupportedLookAndFeelException {
    this.model = model;
    this.N = N;
    this.W = W;
    frame = new JFrame("Evasion");
    Container frameContainer = frame.getContentPane();
    frame.setSize(510, 540);
    frame.setMaximumSize(new Dimension(520, 520));
    frame.setLocation(200, 200);
    //    frame.setExtendedState(JFrame.);
    frame.setVisible(true);
    frame.setResizable(false);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setVisible(true);
    this._init_container(frameContainer);
  }

  */
  @Override
  public void hunter_moved(HunterMove move, final String message) {
    move_counter++;
    hunterPosition = move.position;
    if (move.madeWall) {
      synchronized (lock) {
        walls.add(new Line2D.Float(move.start, move.end));
      }

    }
    else if (move.deletedWall) {
      synchronized (lock) {
        walls.remove(move.wallNumber-1);
      }
    }
    hunterDirection = move.move;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        frame.getContentPane().validate();frame.getContentPane().repaint();
        gameDescription.setText(displayString + move_counter + message);
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_moved(Point2D move) {
    preyPosition = move;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        mainPanel.repaint();
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_caught(long no_of_moves) {
    gameDescription.setText(displayString + ++move_counter + "Hunter Won!!!");
  }

  @Override
  public void game_started(final String hunterName,final String preyName) {
    displayString = "Hunter " + hunterName + " vs Prey " + preyName +". Moves:";
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        mainPanel.repaint();
        frame.repaint();
        gameDescription.setText(displayString + move_counter);
        return 0;
      }
    }.execute();
  }

  @Override
  public void game_reset() {
    

  }

  @Override
  public void time_over(long no_of_moves, final char player) {
    move_counter = no_of_moves;
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        switch (player) {
          case 'h':
            gameDescription.setText(displayString + move_counter +"Hunter Timed Out!!!");
            break;

          case 'p':
            gameDescription.setText(displayString + move_counter+"Prey Timed Out!!!");
            break;

          default:
            break;
        }
        frame.repaint();
        return 0;
      }
    }.execute();
  }

  @Override
  public String getName() {
    return null;
  }

}
