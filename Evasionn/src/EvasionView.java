import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JApplet;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextArea;
import javax.swing.SwingWorker;
import javax.swing.Timer;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.JTextField;


public class EvasionView extends JApplet implements EvasionListener{
  /**
   * 
   */
  public String winner = "not set";
  public String winnerScore = "not set";
  public String getWinner() {
    return winner;
  }

  public String getWinnerScore() {
    return winnerScore;
  }
  Timer imageUpdater;
  private final int multiplier = 2;
  private final long wait = 50;
  private static final long serialVersionUID = 7599154223148422979L;
  private boolean gameWon = true;
  Object lock = new Object();
  Object locker = new Object();
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
      setPreferredSize(new Dimension(this.height, this.width));
      setSize(this.width, this.height);
      setBackground(Color.WHITE);
    }

    public GPanel() {
      this(400, 400);
    }

    @Override
    public void paintComponent(Graphics g) {
      super.paintComponent(g);
      g2d = (Graphics2D)g;
      g2d.setBackground(Color.WHITE);
      //      g2d.clearRect(0, 0, this.height, this.width);
      g2d.setColor(Color.GREEN);
      g2d.setStroke(new BasicStroke(1f));
      synchronized (lock) {
        for (Line2D wall : walls) {
          Line2D line = new Line2D.Double(wall.getX1()*multiplier,wall.getY1()*multiplier,wall.getX2()*multiplier,wall.getY2()*multiplier);
          g2d.draw(line);
        }
      }
      g2d.setColor(Color.BLACK);
      g2d.draw(new Line2D.Float(0,this.width,this.height,this.width));
      g2d.draw(new Line2D.Float(0,0, this.height, 0));
      g2d.draw(new Line2D.Float(0,0,0, this.width));
      g2d.draw(new Line2D.Float(this.width, 0, this.height, this.width));
      if (preyPosition != null) {
        g2d.setColor(Color.BLUE);
        g2d.setStroke(new BasicStroke(3f,BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        Point2D drawPrey = new Point2D.Double(preyPosition.getX()*multiplier,preyPosition.getY()*multiplier);
        g2d.draw(new Line2D.Float(drawPrey,drawPrey));
      }
      if (hunterPosition != null) {
        g2d.setColor(Color.RED);
        g2d.setStroke(new BasicStroke(5f));
        Point2D drawHunter = new Point2D.Double(hunterPosition.getX()*multiplier,hunterPosition.getY()*multiplier);
        g2d.draw(new Line2D.Float(drawHunter,drawHunter));
      }
    }
  }
  EvasionModel model = null;
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
  int N = 3;
  int W = 3;
  int hunter = COMPUTER;
  int prey = COMPUTER;


  /* GUI components */
  JTextField hunterTF = new JTextField(5);
  JTextField preyTF = new JTextField(5);
  JLabel hunterLabel = new JLabel("HunterLabel");
  JLabel preyLabel = new JLabel("PreyLabel");
  String[] nstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox<String> NList = new JComboBox<String>(nstrings);
  String[] wstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox<String> WList = new JComboBox<String>(wstrings);
  ButtonGroup hunterBG = new ButtonGroup();
  JRadioButton hunterComputer = new JRadioButton("Computer");
  JRadioButton hunterHuman = new JRadioButton("Human");
  ButtonGroup preyBG = new ButtonGroup();
  JRadioButton preyComputer = new JRadioButton("Computer");
  JRadioButton preyHuman = new JRadioButton("Human");
  String hunterName = "Computer";
  String preyName = "Computer";
  JButton startButton = new JButton("Start");
  JButton resetButton = new JButton("Reset");


  public void init() {
    resize(550 , 480);
    NList.setSelectedIndex(0);
    NList.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        JComboBox<String> cb = (JComboBox<String>)e.getSource();
        String str = (String)cb.getSelectedItem();
        N = Integer.parseInt(str);
      }
    }
        );

    WList.setSelectedIndex(1);
    WList.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        JComboBox<String> cb = (JComboBox<String>)e.getSource();
        String str = (String)cb.getSelectedItem();
        W = Integer.parseInt(str);
      }
    }
        );

    hunterBG.add(hunterComputer);
    hunterBG.add(hunterHuman);
    hunterComputer.setSelected(true);
    hunterComputer.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if ("Computer".equals(e.getActionCommand())) {
          hunter = COMPUTER;
        }else {
          hunter = HUMAN;
        }
      }
    }
        );
    hunterHuman.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if ("Computer".equals(e.getActionCommand())) {
          hunter = COMPUTER;
        }else {
          hunter = HUMAN;
        }
      }
    }
        );

    preyBG.add(preyComputer);
    preyBG.add(preyHuman);
    preyComputer.setSelected(true);
    preyComputer.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if ("Computer".equals(e.getActionCommand())) {
          prey = COMPUTER;
        }else {
          prey = HUMAN;
        }
      }
    }
        );
    preyHuman.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        if ("Computer".equals(e.getActionCommand())) {
          prey = COMPUTER;
        }else {
          prey = HUMAN;
        }
      }
    }
        );

    resetButton.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        walls = new ArrayList<Line2D>();
        hunter = COMPUTER;
        prey = COMPUTER;
        N = 3;
        W = 3;
        NList.setEnabled(true);
        WList.setEnabled(true);
        hunterHuman.setEnabled(true);
        hunterComputer.setEnabled(true);
        preyHuman.setEnabled(true);
        preyComputer.setEnabled(true);
        hunterComputer.setSelected(true);
        preyComputer.setSelected(true);
        hunterTF.setEnabled(true);
        preyTF.setEnabled(true);
        gameWon = true;
      }
    }
        );
    startButton.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        //        try {
        String tmp = hunterTF.getText();
        if ( tmp != null && tmp != "") {
          hunterName = tmp;
        }
        tmp = preyTF.getText();
        if ( tmp != null && tmp != "") {
          preyName = tmp;
        }
        hunterTF.setEnabled(false);
        preyTF.setEnabled(false);
        NList.setEnabled(false);
        WList.setEnabled(false);
        hunterHuman.setEnabled(false);
        hunterComputer.setEnabled(false);
        preyHuman.setEnabled(false);
        preyComputer.setEnabled(false);
        new SwingWorker<Integer, Integer>() {
          @Override
          protected Integer doInBackground() throws Exception {
            _init_container(getContentPane());
            return 0;
          }
        }.execute();

      }
    }
        );

    Container container = getContentPane();
    container.setBackground(Color.WHITE);
    container.setLayout(new BorderLayout());
    gameDescription = new JTextArea();
    container.add(gameDescription, BorderLayout.PAGE_START);
    JPanel gamePanel = new JPanel();
    container.add(gamePanel, BorderLayout.CENTER);
    gamePanel.setLayout(new FlowLayout());
    mainPanel.setSize(400, 400);
    mainPanel.setBackground(Color.WHITE);
    mainPanel.setVisible(true);
    mainPanel.setFocusable(true);
    gamePanel.add(mainPanel,BorderLayout.CENTER);
    JPanel hunterNamePanel = new JPanel();
    hunterNamePanel.setLayout(new FlowLayout());
    hunterNamePanel.add(new JLabel("HunterName"));
    hunterNamePanel.add(hunterTF);
    JPanel preyNamePanel = new JPanel();
    preyNamePanel.setLayout(new FlowLayout());
    preyNamePanel.add(new JLabel("PreyName"));
    preyNamePanel.add(preyTF);
    JPanel NListPanel = new JPanel();
    NListPanel.setLayout(new FlowLayout());
    NListPanel.add(new JLabel("Choose N"));
    NListPanel.add(NList);
    JPanel WListPanel = new JPanel();
    WListPanel.setLayout(new FlowLayout());
    WListPanel.add(new JLabel("Choose W"));
    WListPanel.add(WList);
    gameDescription.setText("Hunter:" + hunterName + " vs Prey:" + preyName + ". Moves:"+move_counter);
    gameDescription.setEditable(false);
    gamePanel.add(mainPanel);
    JPanel controlPanel = new JPanel();
    controlPanel.add(hunterNamePanel);
    controlPanel.add(preyNamePanel);
    controlPanel.add(NListPanel);
    controlPanel.add(WListPanel);
    gamePanel.add(controlPanel);
    controlPanel.setLayout(new BoxLayout(controlPanel, BoxLayout.Y_AXIS));
    controlPanel.add(hunterLabel);
    controlPanel.add(hunterComputer);
    controlPanel.add(hunterHuman);
    controlPanel.add(preyLabel);
    controlPanel.add(preyComputer);
    controlPanel.add(preyHuman);
    controlPanel.add(startButton);
    controlPanel.add(resetButton);

    try {
      UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
    } catch (ClassNotFoundException e1) {
    } catch (InstantiationException e1) {
    } catch (IllegalAccessException e1) {
    } catch (UnsupportedLookAndFeelException e1) {
    }
    int repaintInterval = 50;
    imageUpdater = new Timer(repaintInterval,
        new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        repaint();
      }
    }
        );
    imageUpdater.start();
    addKeyListener(new KeyHandler());
    setVisible(true);
    setFocusable(true);
  }

  public void _init_container(Container frameContainer) throws ClassNotFoundException, InstantiationException, IllegalAccessException, UnsupportedLookAndFeelException, IOException {
    preyPosition = new Point(130,100);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    time_counter = 0;
    move_counter = 0;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        begin();
        return 0;
      }
    }.execute();
  } 
  Hunter Hunter;
  Prey Prey;
  protected void begin() throws IOException {
    gameWon = false;
    model = new EvasionModel(N, W, 0);
    Hunter = new Hunter(model, N, W);
    Prey = new Prey(model, N, W);
    model.register(Hunter, 'h');
    model.register(Prey, 'p');
    model.register(this, 'v');
    model.hunterReady();
    model.preyReady();
    play();
  }
  long time = System.currentTimeMillis();
  boolean hunterMan;
  boolean preyMan;
  HunterMove hunt_move = new HunterMove();
  PreyMoves prey_move = PreyMoves.ZZ;

  void hunt() {
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        if (hunterMan) {
          hunterMan = false;
          moveHunter(hunt_move);
        }
        else {
          moveHunter();
        }
        return 0;
      }
    }.execute();
  }

  void run() {
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        if (preyMan) {
          preyMan = false;
          movePrey(prey_move);
        }
        else {
          movePrey();
        }
        return 0;
      }
    }.execute();
  }

  private void play() throws IOException {
    setVisible(false);
    setVisible(true);
    requestFocusInWindow();

    while(!gameWon) {
      time = System.currentTimeMillis();
      while(System.currentTimeMillis() - time < wait);
      hunt();
      time = System.currentTimeMillis();
      while(System.currentTimeMillis() - time < wait + 30);
      hunt();
      run();
    }
  }
  private boolean huntKey = false;
  private boolean preyKey = false;
  private synchronized void moveHunter() throws IOException {
    if (huntKey) {
      huntKey = false;
      return;
    }
    HunterMove move = new HunterMove();
    move.move = hunterDirection;
    long val = System.currentTimeMillis() - time;
    Hunter.make_move(move, val > wait? 0 : val);
  }

  private synchronized void movePrey() throws IOException {
    if (preyKey) {
      preyKey = false;
      return;
    }
    long val = System.currentTimeMillis() - time;
    Prey.make_move(PreyMoves.ZZ, val > wait? 0 : val);
  }
  private synchronized void moveHunter(HunterMove move) throws IOException {
    if (huntKey) {
      huntKey = false;
      return;
    }
    long val = System.currentTimeMillis() - time;
    Hunter.make_move(move, val > wait? 0 : val);
  }

  private synchronized void movePrey(PreyMoves move) throws IOException {
    if (preyKey) {
      preyKey = false;
      return;
    }
    long val = System.currentTimeMillis() - time;
    Prey.make_move(move, val > wait? 0 : val);
  }

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
    gameDescription.setText("Hunter:" + hunterName + "vs Prey:" + preyName + ". Moves:"+move_counter);
  }

  @Override
  public void prey_moved(Point2D move) {
    preyPosition = move;
  }

  @Override
  public void prey_caught(long no_of_moves) {
    gameWon = true;
    gameDescription.setText("Hunter:" + hunterName + "vs Prey:" + preyName + ". Moves:"+move_counter + "Game Over");
    winnerScore = move_counter+"";
    winner = "Hunter";
  }

  @Override
  public void game_started(final String hunterName,final String preyName) {
    gameDescription.setText("Hunter:" + hunterName + "vs Prey:" + preyName + ". Moves:"+move_counter);
  }

  @Override
  public void game_reset() {


  }

  @Override
  public void time_over(long no_of_moves, final char player) {
    move_counter = no_of_moves;
    switch (player) {
    case 'h':
      gameDescription.setText("Hunter:" + hunterName + "vs Prey:" + preyName + ". Moves:"+move_counter);
      break;

    case 'p':
      gameDescription.setText("Hunter:" + hunterName + "vs Prey:" + preyName + ". Moves:"+move_counter);
      break;

    default:
      break;
    }
  }

  @Override
  public String getName() {
    return null;
  }

  private Point2D.Float getIntersectionPoint(Line2D.Float line1, Line2D.Float line2) {
    if (! line1.intersectsLine(line2) ) return null;
    double px = line1.getX1(),
        py = line1.getY1(),
        rx = line1.getX2()-px,
        ry = line1.getY2()-py;
    double qx = line2.getX1(),
        qy = line2.getY1(),
        sx = line2.getX2()-qx,
        sy = line2.getY2()-qy;

    double det = sx*ry - sy*rx;
    if (det == 0) {
      return null;
    } else {
      double z = (sx*(qy-py)+sy*(px-qx))/det;
      if (z==0 ||  z==1) return null;  // intersection at end point!
      return new Point2D.Float(
          (float)(px+z*rx), (float)(py+z*ry));
    }
  }

  private class KeyHandler implements KeyListener {

    @Override
    public void keyPressed(KeyEvent e) {
      if (gameWon) {
        return;
      }
      //      synchronized (locker) {
      keyPressed = true;
      char c = e.getKeyChar();
      int a = c;
      PreyMoves pMove = PreyMoves.ZZ;
      HunterMove hMove = new HunterMove();
      hMove.move = hunterDirection;
      switch(c) {
      case 'h':
      case 'H':
        Line2D hWall = new Line2D.Float(0, (float)hunterPosition.getY(), 199, (float)hunterPosition.getY());
        {
          float top = -1;
          float bottom = 200;

          for (Line2D wall : walls) {
            if (hWall.intersectsLine(wall)) {
              Point2D intersect = getIntersectionPoint((Line2D.Float)hWall, (Line2D.Float)wall);
              float x = (float)intersect.getX();
              if (x < hunterPosition.getX()) {
                if (x > top) {
                  top = x;
                }
              }
              else if (x > hunterPosition.getX()) {
                if (x < bottom) {
                  bottom = x;
                }
              }
            }
          }
          hMove.start = new Point2D.Float(++top, (float)hunterPosition.getY());
          hMove.end = new Point2D.Float(--bottom, (float)hunterPosition.getY());
        }
        hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "w");
        hunt_move = hMove;
        hunterMan = true;
        break;

      case 'v':
      case 'V':
        Line2D vWall = new Line2D.Float((float)hunterPosition.getX(), 0, (float)hunterPosition.getX(), 199);
        {
          float top = -1;
          float bottom = 200;

          for (Line2D wall : walls) {
            if (vWall.intersectsLine(wall)) {
              Point2D intersect = getIntersectionPoint((Line2D.Float)vWall, (Line2D.Float)wall);
              float x = (float)intersect.getY();
              if (x < hunterPosition.getY()) {
                if (x > top) {
                  top = x;
                }
              }
              else if (x > hunterPosition.getY()) {
                if (x < bottom) {
                  bottom = x;
                }
              }
            }
          }
          hMove.start = new Point2D.Float((float)hunterPosition.getX(),++top);
          hMove.end = new Point2D.Float((float)hunterPosition.getX(),--bottom);
        }
        hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "w");
        hunt_move = hMove;
        hunterMan = true;
        break;

      case 'z':
      case 'Z':
        hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "wx");
        hMove.wallNumber = 1;
        hunt_move = hMove;
        hunterMan = true;
        break;

      case 'x':
      case 'X':
        hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "wx");
        hMove.wallNumber = walls.size();
        hunt_move = hMove;
        hunterMan = true;
        break;

      case KeyEvent.VK_UP:  
      case KeyEvent.VK_KP_UP:
      case 'o':
      case 'O':
        pMove = PreyMoves.NN;
        prey_move = pMove;
        preyMan = true;
        break;
      case KeyEvent.VK_DOWN:  
      case KeyEvent.VK_KP_DOWN:
      case '.':
      case '>':
        pMove = PreyMoves.SS;
        prey_move = pMove;
        preyMan = true;
        break;
      case KeyEvent.VK_LEFT:  
      case KeyEvent.VK_KP_LEFT:
      case 'k':
      case 'K':
        pMove = PreyMoves.WW;
        prey_move = pMove;
        preyMan = true;
        break;
      case KeyEvent.VK_RIGHT:  
      case KeyEvent.VK_KP_RIGHT:
      case ';':
      case ':':
        pMove = PreyMoves.EE;
        prey_move = pMove;
        preyMan = true;
        break;
      case KeyEvent.VK_NUMPAD1:
      case ',':
      case '<':
        pMove = PreyMoves.SW;
        prey_move = pMove;
        preyMan = true;
        break;
      case KeyEvent.VK_NUMPAD3:
      case '/':
      case '?':
        pMove = PreyMoves.SE;
        prey_move = pMove;
        preyMan = true;
        break;
      case KeyEvent.VK_NUMPAD7:
      case 'i':
      case 'I':
        pMove = PreyMoves.NW;
        prey_move = pMove;
        preyMan = true;
        break;
      case 'p':
      case 'P':
        pMove = PreyMoves.NE;
        prey_move = pMove;
        preyMan = true;
        break;
      }
    }
    
    @Override
    public void keyTyped(KeyEvent e) {
    }

    @Override
    public void keyReleased(KeyEvent e) {

    }
  }
  public void start() {
    this.requestFocusInWindow();
  }
  boolean keyPressed;
}
