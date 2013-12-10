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
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;


public class EvasionView extends JApplet implements EvasionListener, KeyListener{
  /**
   * 
   */
//  EvasionModel model;
  private final int multiplier = 2;
  private final long wait = 50;
  private static final long serialVersionUID = 7599154223148422979L;
  private boolean gameWon = true;
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

  String hunterName;
  String preyName;

  /* GUI components */
  JLabel displayString = new JLabel("DisplayLabel");
  JLabel hunterLabel = new JLabel("HunterLabel");
  JLabel preyLabel = new JLabel("PreyLabel");

  String[] nstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox<String> NList = new JComboBox<String>(nstrings);
  //

  //

  String[] wstrings = {"3", "4", "5", "6", "7", "8", "9", "10"};
  JComboBox<String> WList = new JComboBox<String>(wstrings);
  //

  //
//  class RadioActionListener implements ActionListener {
//    int id = 0;
//    public RadioActionListener(int id) {
//      this.id = id;
//    }
//    public void actionPerformed(ActionEvent e) {
//      if(this.id == 0) {// hunter
//        if ("Computer".equals(e.getActionCommand())) {
//          super.hunter = COMPUTER;
//        }else {
//          super.hunter = HUMAN;
//        }
//      }
//      else {
//        if ("Computer".equals(e.getActionCommand())) {
//          super.prey = COMPUTER;
//        }else {
//          super.prey = HUMAN;
//        }
//      }
//    }
//  }
  
  
  ButtonGroup hunterBG = new ButtonGroup();
  JRadioButton hunterComputer = new JRadioButton("Computer");
  JRadioButton hunterHuman = new JRadioButton("Human");
  //

  //
  ButtonGroup preyBG = new ButtonGroup();
  JRadioButton preyComputer = new JRadioButton("Computer");
  JRadioButton preyHuman = new JRadioButton("Human");
  //

  //
  JButton startButton = new JButton("Start");
  JButton resetButton = new JButton("Reset");
  //

  //
  public void init() {
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
        hunterComputer.setSelected(true);
        preyComputer.setSelected(true);
        gameWon = true;
        repaint();
      }
    }
        );
    startButton.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e) {
//        try {
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
    
    //
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
      // TODO Auto-generated catch block
      e1.printStackTrace();
    } catch (InstantiationException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    } catch (IllegalAccessException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    } catch (UnsupportedLookAndFeelException e1) {
      // TODO Auto-generated catch block
      e1.printStackTrace();
    }
  }

  public void _init_container(Container frameContainer) throws ClassNotFoundException, InstantiationException, IllegalAccessException, UnsupportedLookAndFeelException, IOException {
    preyPosition = new Point(130,100);
    hunterPosition = new Point(0, 0);
    moves_to_next_wall = N;
    time_counter = 0;
    move_counter = 0;
    hunterDirection = HunterMoves.SE;
    walls = new ArrayList<Line2D>();

    UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
    gameDescription = new JTextArea();
    mainPanel = new GPanel();
    mainPanel.setSize(400, 400);
    mainPanel.setBackground(Color.WHITE);
    //    mainPanel.setBovvv   rder(BorderFactory.createLineBorder(Color.black, 1));
    mainPanel.setVisible(true);
    mainPanel.setFocusable(true);
    frameContainer.add(mainPanel,BorderLayout.CENTER);
    frameContainer.add(gameDescription,BorderLayout.NORTH);
//    new SwingWorker<Integer, Integer>() {
//      @Override
//      protected Integer doInBackground() throws Exception {
        frameContainer.repaint();
        gameDescription.setText(displayString.getText() + move_counter);
        begin();
//        return 0;
//      }
//    }.execute();
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
  
  private void play() throws IOException {
    while(!gameWon) {
      time = System.currentTimeMillis();
      while(System.currentTimeMillis() - time < wait);
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          moveHunter();
          return 0;
        }
      }.execute();
      time = System.currentTimeMillis();
      while(System.currentTimeMillis() - time < wait + 30);
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          moveHunter();
          return 0;
        }
      }.execute();
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          Thread.sleep(1);
          movePrey();
          return 0;
        }
      }.execute();
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
//    HunterMove move = new HunterMove();
    move.move = hunterDirection;
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
        repaint();
        gameDescription.setText(displayString.getText() + move_counter + message);
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
        repaint();
        return 0;
      }
    }.execute();
  }

  @Override
  public void prey_caught(long no_of_moves) {
    gameWon = true;
    gameDescription.setText(displayString.getText() + ++move_counter + "Hunter Won!!!");
  }

  @Override
  public void game_started(final String hunterName,final String preyName) {
    displayString.setText("Hunter " + hunterName + " vs Prey " + preyName +". Moves:");
    new SwingWorker<Integer, Integer>() {
      @Override
      protected Integer doInBackground() throws Exception {
        repaint();
        gameDescription.setText(displayString.getText() + move_counter);
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
          gameDescription.setText(displayString.getText() + move_counter +"Hunter Timed Out!!!");
          break;

        case 'p':
          gameDescription.setText(displayString.getText() + move_counter+"Prey Timed Out!!!");
          break;

        default:
          break;
        }
        repaint();
        return 0;
      }
    }.execute();
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
  
  
  @Override
  public void keyPressed(KeyEvent e) {
    char c = e.getKeyChar();
    PreyMoves pMove = PreyMoves.ZZ;
    HunterMove hMove = new HunterMove();
    hMove.move = hunterDirection;
    switch(c) {
    case KeyEvent.VK_H:
      Line2D hWall = new Line2D.Float(0, (float)hunterPosition.getY(), 199, (float)hunterPosition.getY());
      {
        float top = 0;
        float bottom = 199;

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
        hMove.start = new Point2D.Float(top, (float)hunterPosition.getY());
        hMove.start = new Point2D.Float(bottom, (float)hunterPosition.getY());
      }
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + 'w');
      break;
      
    case KeyEvent.VK_V:
      Line2D vWall = new Line2D.Float((float)hunterPosition.getX(), 0, (float)hunterPosition.getX(), 199);
      {
        float top = 0;
        float bottom = 199;

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
        hMove.start = new Point2D.Float((float)hunterPosition.getX(),top);
        hMove.start = new Point2D.Float((float)hunterPosition.getX(),bottom);
      }
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + 'w');
      break;
     
    case KeyEvent.VK_Z:
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "wx");
      hMove.wallNumber = 0;
      break;
      
    case KeyEvent.VK_X:
      hMove.move = HunterMoves.valueOf(hunterDirection.toString() + "wx");
      hMove.wallNumber = walls.size() - 1;
      break;
      
    case KeyEvent.VK_UP:
      pMove = PreyMoves.NN;
      break;
    case KeyEvent.VK_DOWN:
      pMove = PreyMoves.SS;
      break;
    case KeyEvent.VK_LEFT:
      pMove = PreyMoves.WW;
      break;
    case KeyEvent.VK_RIGHT:
      pMove = PreyMoves.EE;
      break;
    case KeyEvent.VK_NUMPAD1:
      pMove = PreyMoves.SW;
      break;
    case KeyEvent.VK_NUMPAD3:
      pMove = PreyMoves.SE;
      break;
    case KeyEvent.VK_NUMPAD7:
      pMove = PreyMoves.NW;
      break;
    case KeyEvent.VK_NUMPAD9:
      pMove = PreyMoves.NE;
      break;
    }
    if (pMove != PreyMoves.ZZ) {
      final HunterMove move = hMove;
      move.move = hunterDirection;
      huntKey = true;
//      final long val = System.currentTimeMillis() - time;
      
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          moveHunter(move);
          return 0;
        }
      }.execute();
    }
    else {
      final PreyMoves move = pMove;
      preyKey = true;
//      final long val = System.currentTimeMillis() - time;
      new SwingWorker<Integer, Integer>() {
        @Override
        protected Integer doInBackground() throws Exception {
          movePrey(move);
          return 0;
        }
      }.execute();
    }
  }

  @Override
  public void keyTyped(KeyEvent e) {
    // TODO Auto-generated method stub
    int j = 50;
  }

  @Override
  public void keyReleased(KeyEvent e) {
    // TODO Auto-generated method stub
    int j =30;
  }
  
  
}
