import java.util.ArrayList;

public class PlaylistNodeSecondaryIndex extends PlaylistNode {
	private ArrayList<String> genres;
	private ArrayList<PlaylistNode> children;

	public PlaylistNodeSecondaryIndex(PlaylistNode parent) {
		super(parent);
		genres = new ArrayList<String>();
		children = new ArrayList<PlaylistNode>();
		this.type = PlaylistNodeType.Internal;
	}
	
	public PlaylistNodeSecondaryIndex(PlaylistNode parent, ArrayList<String> genres, ArrayList<PlaylistNode> children) {
		super(parent);
		this.genres = genres;
		this.children = children;
		this.type = PlaylistNodeType.Internal;
	}
	
	// GUI Methods - Do not modify
	public ArrayList<PlaylistNode> getAllChildren()
	{
		return this.children;
	}
	
	public PlaylistNode getChildrenAt(Integer index) {
		
		return this.children.get(index);
	}
	

	public Integer genreCount()
	{
		return this.genres.size();
	}
	
	public String genreAtIndex(Integer index) {
		if(index >= this.genreCount() || index < 0) {
			return "Not Valid Index!!!";
		}
		else {
			return this.genres.get(index);
		}
	}
	
	
	// Extra functions if needed
	public boolean inttobool(int a)  //toconvert an integer value to a boolean value.
	{
		boolean boolValue;
		if (a<0) {
			boolValue = true;
		}
		else {
			boolValue = false;
		}
		return boolValue;
	}
	public void addSong(CengSong song){
		// if its children are nodes
		if (this.children.get(0).type == PlaylistNodeType.Internal){
			Integer length, i=0;
			length = this.genres.size();
			for(i=0;  i<length && inttobool(((this.genres.get(i)).compareTo(song.genre()))); i++); //(s1.compareTo(s3));  1(because s1>s3)   this.genres.get(i)<song.genre()
			if(i<length && (this.genres.get(i)).equals(song.genre()))
			{
				return;
			} //(songs.get(i).genre()).equals(song.genre())
			((PlaylistNodeSecondaryIndex)this.children.get(i)).addSong(song);
		}
		// if its children are leaves
		else{
			Integer length, length2, i, j;
			length = this.genres.size();

			for(i=0;  i<length && inttobool((this.genres.get(i)).compareTo(song.genre())); i++);  //(song.genre).compareTo(this.genres.get(i))
			if(i<length && (this.genres.get(i)).equals(song.genre()))
			{
				return;
			}
			PlaylistNodeSecondaryLeaf lef;
			lef = ((PlaylistNodeSecondaryLeaf)this.children.get(i));
			length2 = lef.genreCount();
			// if it is not full

			if (((PlaylistNodeSecondaryLeaf)this.children.get(i)).genreCount() < 2*PlaylistNode.order){
				for(j=0; j<length2 && inttobool((lef.genreAtIndex(j)).compareTo(song.genre())); j++); //lef.genreAtIndex(j) ((song.genre()).compareTo(songs.get(j).genre()))
				if (j<length2 && (lef.genreAtIndex(j)).equals(song.genre()))
				{
					((PlaylistNodeSecondaryLeaf)this.children.get(i)).addSong(j,song);
					return;
				}
				((PlaylistNodeSecondaryLeaf)this.children.get(i)).addSong(j,song);
			}
			// if it is full

			else{
				PlaylistNodeSecondaryLeaf newChild1, newChild2;
				String newNodeGenre;
				for(j=0; j<length2 && inttobool((lef.genreAtIndex(j)).compareTo(song.genre())); j++);
				if (j<length2 && (lef.genreAtIndex(j)).equals(song.genre()))
				{
					((PlaylistNodeSecondaryLeaf)this.children.get(i)).addSong(j,song);
					return;
				}
				lef.addSong(i, song);
				newChild1 = new PlaylistNodeSecondaryLeaf(null, (new ArrayList<ArrayList<CengSong>> (lef.getSongBucket().subList(0,PlaylistNode.order))));
				newChild2 = new PlaylistNodeSecondaryLeaf(null, (new ArrayList<ArrayList<CengSong>> (lef.getSongBucket().subList(PlaylistNode.order,2*PlaylistNode.order+1))));
				newNodeGenre = lef.genreAtIndex(PlaylistNode.order);
				this.addGenreAnd2Children(newNodeGenre, newChild1, newChild2);
			}
		}
	}

	private void addGenreAnd2Children(String newNodeGenre,  PlaylistNode newChild1, PlaylistNode newChild2){
		// if it is not full -- uppernode
		if(this.genreCount() < 2*PlaylistNode.order){
			Integer length, i;
			ArrayList<PlaylistNode> newChildren = this.children;
			length = this.genres.size();
			for(i=0;  i<length && inttobool(this.genres.get(i).compareTo(newNodeGenre)); i++);
			if(i<length && this.genres.get(i).equals(newNodeGenre))
			{
				return;
			}
			this.genres.add(i, newNodeGenre);
			newChild1.setParent(this);
			newChild2.setParent(this);
			newChildren.set(i, newChild1);
			newChildren.add(i+1, newChild2);
			this.children = newChildren;
		}
		// if it is full
		else{
			ArrayList<String> newGenres = this.genres;
			ArrayList<PlaylistNode> newChildren = this.children;
			Integer length, i;
			String parentNewGenre;
			PlaylistNodeSecondaryIndex parentNewChild;
			length = genres.size();
			for(i=0;  i<length && inttobool(this.genres.get(i).compareTo(newNodeGenre)); i++);
			if(i<length && this.genres.get(i).equals(newNodeGenre))
			{
				return;
			}
			newGenres.add(i,newNodeGenre);
			newChildren.set(i, newChild1);
			newChildren.add(i+1, newChild2);
			this.genres = new ArrayList<String> (newGenres.subList(0,PlaylistNode.order));
			parentNewGenre = newGenres.get(PlaylistNode.order);
			this.children = new ArrayList<PlaylistNode> (newChildren.subList(0,PlaylistNode.order+1));
			parentNewChild = new PlaylistNodeSecondaryIndex(null, (new ArrayList<String> (newGenres.subList(PlaylistNode.order+1,2*PlaylistNode.order+1))) ,
					(new ArrayList<PlaylistNode> (newChildren.subList(PlaylistNode.order+1,2*PlaylistNode.order+2)))); //parent,genres,children
			if (i+1 < PlaylistNode.order){
				newChild1.setParent(this);
				newChild2.setParent(this);
			}
			else if (i < PlaylistNode.order){
				newChild1.setParent(this);
				newChild2.setParent(parentNewChild);
			}
			else {
				newChild1.setParent(parentNewChild);
				newChild2.setParent(parentNewChild);
			}

			if(this.getParent() == null){
				ArrayList<PlaylistNode> newParentChildren;
				ArrayList<String> newParentGenres;
				newParentChildren = new ArrayList<PlaylistNode>();
				newParentChildren.add((PlaylistNode) this);
				newParentChildren.add((PlaylistNode) parentNewChild);
				newParentGenres = new ArrayList<String>();
				newParentGenres.add(parentNewGenre);
				this.setParent((PlaylistNode) new PlaylistNodeSecondaryIndex(null, newParentGenres, newParentChildren)); //parent,genres,children
				parentNewChild.setParent(this.getParent());
			}
			else{
				((PlaylistNodeSecondaryIndex)this.getParent()).addGenreand1Child(parentNewGenre, parentNewChild);
			}
		}
	}

	private void addGenreand1Child(String nodeNewGenre,  PlaylistNodeSecondaryIndex newChild){
		// if it is not full
		if(this.genreCount() < 2*PlaylistNode.order){
			Integer length, i;
			ArrayList<PlaylistNode> newChildren = this.children;
			length = this.genres.size();
			for(i=0;  i<length && inttobool(this.genres.get(i).compareTo(nodeNewGenre)); i++);
			if(this.genres.get(i).equals(nodeNewGenre))
			{
				return;
			}
			this.genres.add(i, nodeNewGenre);
			newChild.setParent(this);
			newChildren.add(i+1, newChild);
			this.children = newChildren;
		}
		// if it is full
		else{
			Integer length, i;
			String parentNewGenre;
			ArrayList<PlaylistNode> newChildren = this.children;
			ArrayList<String> newGenres = this.genres;
			PlaylistNodeSecondaryIndex parentNewChild;
			length = this.genres.size();
			for(i=0;  i<length && inttobool(this.genres.get(i).compareTo(nodeNewGenre)); i++);
			if(this.genres.get(i).equals(nodeNewGenre))
			{
				return;
			}
			newGenres.add(i, nodeNewGenre);
			newChildren.add(i+1, newChild);
			this.genres = new ArrayList<String> (newGenres.subList(0,PlaylistNode.order));
			parentNewGenre = newGenres.get(PlaylistNode.order);
			this.children = new ArrayList<PlaylistNode> (newChildren.subList(0,PlaylistNode.order+1));
			parentNewChild = new PlaylistNodeSecondaryIndex(null, (new ArrayList<String> (newGenres.subList(PlaylistNode.order+1,2*PlaylistNode.order+1))),
					(new ArrayList<PlaylistNode> (newChildren.subList(PlaylistNode.order+1,2*PlaylistNode.order+2)))); //parent, genre, children
			if (i+1 < PlaylistNode.order){
				newChild.setParent(this);
			}
			else {
				newChild.setParent(parentNewChild);
			}
			if(this.getParent() == null){
				ArrayList<PlaylistNode> newParentChildren;
				ArrayList<String> newParentGenres;
				newParentChildren = new ArrayList<PlaylistNode>();
				newParentChildren.add(this);
				newParentChildren.add(parentNewChild);
				newParentGenres = new ArrayList<String>();
				newParentGenres.add(parentNewGenre);
				this.setParent(new PlaylistNodeSecondaryIndex(null, newParentGenres, newParentChildren)); //parent, genres, children
				parentNewChild.setParent(this.getParent());
			}
			else{
				((PlaylistNodeSecondaryIndex)this.getParent()).addGenreand1Child(parentNewGenre, parentNewChild);
			}
		}
	}
	public String addtab(int indent) {
		String s = "";
		for (int i = 0; i < indent; i++) {
			s += "\t";
		}
		return s;
	}

	public void printIndent(int indent) {
		System.out.print(this.addtab(indent)); //lnyi
	}
	public void nodeindex(int indent) {
		this.printIndent(indent);
		System.out.println("<index>");

		for (int i = 0; i < this.genres.size(); i++) {
			this.printIndent(indent);
			System.out.println(this.genres.get(i));
		}

		this.printIndent(indent);
		System.out.println("</index>");
	}
	public void printSong(String genre,CengSong sons, int indent) {
		indent++;
		indent++;
		this.printIndent(indent);
		System.out.println("<record>" + sons.audioId() + "|" + sons.genre() + "|" + sons.songName() + "|" + sons.artist() + "</record>");
	}
	public void print(int indent){
		this.nodeindex(indent++);
		indent++;
		// if its children are nodes
		if (this.children.get(0).getType() == PlaylistNodeType.Internal){
			int size = this.children.size();
			for (int i = 0; i < size; i++) {
				this.print(indent);
			}
		}
		// if its children are leaves
		else{
			for(PlaylistNode child:this.children){
				PlaylistNodeSecondaryLeaf currentLeaf;
				currentLeaf = (PlaylistNodeSecondaryLeaf) child;
				int length = currentLeaf.genreCount();
				printIndent(indent);
				System.out.println("<data>");

				for(int i=0; i<length; i++)
				{
					printIndent(indent);
					System.out.println(currentLeaf.genreAtIndex(i));
					ArrayList<CengSong> sons = currentLeaf.getSongBucket().get(i);
					int length2 = sons.size();
					for(int j = 0; j<length2; j++)
					{

						this.printSong(currentLeaf.genreAtIndex(j), sons.get(j), indent);
					}
				}
				printIndent(indent);
				System.out.println("</data>");
			}
		}
	}

}
