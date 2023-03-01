import java.util.ArrayList;

public class PlaylistNodePrimaryIndex extends PlaylistNode {
	private ArrayList<Integer> audioIds;
	private ArrayList<PlaylistNode> children;

	public PlaylistNodePrimaryIndex(PlaylistNode parent) {
		super(parent);
		audioIds = new ArrayList<Integer>();
		children = new ArrayList<PlaylistNode>();
		this.type = PlaylistNodeType.Internal;
	}

	public PlaylistNodePrimaryIndex(PlaylistNode parent, ArrayList<Integer> audioIds, ArrayList<PlaylistNode> children) {
		super(parent);
		this.audioIds = audioIds;
		this.children = children;
		this.type = PlaylistNodeType.Internal;
	}

	// GUI Methods - Do not modify
	public ArrayList<PlaylistNode> getAllChildren() {
		return this.children;
	}

	public PlaylistNode getChildrenAt(Integer index) {
		return this.children.get(index);
	}

	public Integer audioIdCount() {
		return this.audioIds.size();
	}

	public Integer audioIdAtIndex(Integer index) {
		if (index >= this.audioIdCount() || index < 0) {
			return -1;
		} else {
			return this.audioIds.get(index);
		}
	}

	// Extra functions if needed
	public void addSong(CengSong song) {
		//if children -> internal
		if (this.children.get(0).type == PlaylistNodeType.Internal) {
			int length, i;
			length = this.audioIds.size();
			for (i = 0; i < length && this.audioIds.get(i) < song.audioId(); i++) ;
			((PlaylistNodePrimaryIndex) this.children.get(i)).addSong(song);

		}
		//if children->leaves
		else {
			ArrayList<CengSong> songs;
			int length, i;
			length = this.audioIds.size();
			for (i = 0; i < length && this.audioIds.get(i) < song.audioId(); i++) ;
			//not fulll
			if (((PlaylistNodePrimaryLeaf) this.children.get(i)).songCount() < 2 * PlaylistNode.order) {
				int j;
				songs = ((PlaylistNodePrimaryLeaf) this.children.get(i)).getSongs();
				length = songs.size();
				for (j = 0; j < length && songs.get(j).audioId() < song.audioId(); j++) ;
				((PlaylistNodePrimaryLeaf) this.children.get(i)).addSong(j, song);
			}
			//it's full
			else {
				PlaylistNodePrimaryLeaf newChild1, newChild2, currentLeaf;
				int newNodeKey, j;
				currentLeaf = (PlaylistNodePrimaryLeaf) this.children.get(i);
				songs = currentLeaf.getSongs();
				length = songs.size();
				for (j = 0; j < length && songs.get(j).audioId() < song.audioId(); j++) ;
				songs.add(j, song);
				newChild1 = new PlaylistNodePrimaryLeaf(null, (new ArrayList<CengSong>(songs.subList(0, PlaylistNode.order))));
				newChild2 = new PlaylistNodePrimaryLeaf(null, (new ArrayList<CengSong>(songs.subList(PlaylistNode.order, 2 * PlaylistNode.order + 1))));
				newNodeKey = songs.get(PlaylistNode.order).audioId();
				this.addchildren(newNodeKey, newChild1, newChild2);
			}
		}
	}

	private void addchildren(int nodeNewKey, PlaylistNode newChild1, PlaylistNode newChild2) {
		// if it is not full
		if (this.audioIdCount() < 2 * PlaylistNode.order) {
			int length, i;
			ArrayList<PlaylistNode> newChildren = this.children;
			length = this.audioIds.size();
			for (i = 0; i < length && this.audioIds.get(i) < nodeNewKey; i++) ;
			this.audioIds.add(i, nodeNewKey);
			newChild1.setParent(this);
			newChild2.setParent(this);
			newChildren.set(i, newChild1);
			newChildren.add(i + 1, newChild2);
			this.children = newChildren;
		}
		// if it is full
		else {
			ArrayList<Integer> newKeys = this.audioIds;
			ArrayList<PlaylistNode> newChildren = this.children;
			int length, i, parentNewKey;
			PlaylistNodePrimaryIndex parentNewChild;
			length = audioIds.size();
			for (i = 0; i < length && newKeys.get(i) < nodeNewKey; i++) ;
			newKeys.add(i, nodeNewKey);
			newChildren.set(i, newChild1);
			newChildren.add(i + 1, newChild2);
			this.audioIds = new ArrayList<Integer>(newKeys.subList(0, PlaylistNode.order));
			parentNewKey = newKeys.get(PlaylistNode.order);
			this.children = new ArrayList<PlaylistNode>(newChildren.subList(0, PlaylistNode.order + 1));
			parentNewChild = new PlaylistNodePrimaryIndex(null, (new ArrayList<Integer>(newKeys.subList(PlaylistNode.order + 1, 2 * PlaylistNode.order + 1))), (new ArrayList<PlaylistNode>(newChildren.subList(PlaylistNode.order + 1, 2 * PlaylistNode.order + 2))));
			if (i + 1 < PlaylistNode.order) {
				newChild1.setParent(this);
				newChild2.setParent(this);
			} else if (i < PlaylistNode.order) {
				newChild1.setParent(this);
				newChild2.setParent(parentNewChild);
			} else {
				newChild1.setParent(parentNewChild);
				newChild2.setParent(parentNewChild);
			}

			if (this.getParent() == null) {
				ArrayList<PlaylistNode> newParentChildren;
				ArrayList<Integer> newParentKeys;
				newParentChildren = new ArrayList<PlaylistNode>();
				newParentChildren.add((PlaylistNode) this);
				newParentChildren.add((PlaylistNode) parentNewChild);
				newParentKeys = new ArrayList<Integer>();
				newParentKeys.add(parentNewKey);
				this.setParent((PlaylistNode) new PlaylistNodePrimaryIndex(null, newParentKeys, newParentChildren));
				parentNewChild.setParent(this.getParent());
			} else {
				((PlaylistNodePrimaryIndex) this.getParent()).addchild(parentNewKey, parentNewChild);
			}
		}
	}

	private void addchild(int nodeNewKey, PlaylistNodePrimaryIndex newChild)
	{
		// if it is not full
		if (this.audioIdCount() < 2 * PlaylistNode.order) {
			int length, i;
			ArrayList<PlaylistNode> newChildren = this.children;
			length = this.audioIds.size();
			for (i = 0; i < length && this.audioIds.get(i) < nodeNewKey; i++) ;
			this.audioIds.add(i, nodeNewKey);
			newChild.setParent(this);
			newChildren.add(i + 1, newChild);
			this.children = newChildren;
		}
		// if it is full
		else {
			int length, i, parentNewKey;
			ArrayList<PlaylistNode> newChildren = this.children;
			ArrayList<Integer> newKeys = this.audioIds;
			PlaylistNodePrimaryIndex parentNewChild;
			length = this.audioIds.size();
			for (i = 0; i < length && this.audioIds.get(i) < nodeNewKey; i++) ;
			newKeys.add(i, nodeNewKey);
			newChildren.add(i + 1, newChild);
			this.audioIds = new ArrayList<Integer>(newKeys.subList(0, PlaylistNode.order));
			parentNewKey = newKeys.get(PlaylistNode.order);
			this.children = new ArrayList<PlaylistNode>(newChildren.subList(0, PlaylistNode.order + 1));
			parentNewChild = new PlaylistNodePrimaryIndex(null, (new ArrayList<Integer>(newKeys.subList(PlaylistNode.order + 1, 2 * PlaylistNode.order + 1))), (new ArrayList<PlaylistNode>(newChildren.subList(PlaylistNode.order + 1, 2 * PlaylistNode.order + 2))));
			if (i + 1 < PlaylistNode.order) {
				newChild.setParent(this);
			} else {
				newChild.setParent(parentNewChild);
			}
			if (this.getParent() == null) {
				ArrayList<PlaylistNode> newParentChildren;
				ArrayList<Integer> newParentKeys;
				newParentChildren = new ArrayList<PlaylistNode>();
				newParentChildren.add(this);
				newParentChildren.add(parentNewChild);
				newParentKeys = new ArrayList<Integer>();
				newParentKeys.add(parentNewKey);
				this.setParent(new PlaylistNodePrimaryIndex(null, newParentKeys, newParentChildren));
				parentNewChild.setParent(this.getParent());
			} else {
				((PlaylistNodePrimaryIndex) this.getParent()).addchild(parentNewKey, parentNewChild);
			}
		}


	}
	public CengSong searchSong(int audioId, int indent){
		PlaylistNode result = null;
		int length = this.audioIds.size(), i;
		this.printIndent(indent);
		System.out.println("<index>");
		for(i=0;  i<length; i++){
			this.printIndent(indent);
			System.out.println(this.audioIds.get(i));
			if (this.audioIds.get(i)<=audioId){
				result = children.get(i+1);
			}
		}
		this.printIndent(indent);
		System.out.println("</index>");
		indent++;
		// if its children are nodes
		if (this.children.get(0).type == PlaylistNodeType.Internal){
			PlaylistNodePrimaryIndex currentNode;
			currentNode = (PlaylistNodePrimaryIndex) result;
			if (result!=null){
				return currentNode.searchSong(audioId, indent);
			}
			else{
				return ((PlaylistNodePrimaryIndex)this.children.get(0)).searchSong(audioId, indent);
			}
		}
		// if its children are leaves
		else{
			ArrayList<CengSong> songs;
			PlaylistNodePrimaryLeaf currentLeaf;
			if (result==null){
				result = children.get(0);
			}
			currentLeaf = (PlaylistNodePrimaryLeaf) result;
			songs = currentLeaf.getSongs();
			length = songs.size();
			for(i=0; i<length; i++){
				if(songs.get(i).audioId()==audioId){
					printIndent(indent);
					System.out.print("<data>");
					printSong(songs.get(i), indent);
					printIndent(indent);
					System.out.print("</data>");
					return songs.get(i);
				}
			}
			System.out.print("Could not find ");
			System.out.println(audioId);
			System.out.println(".");
			return null;
		}
	}
	public String createIndent(int indent) {
		String s = "";
		for (int i = 0; i < indent; i++) {
			s += "\t";
		}
		return s;
	}

	public void printIndent(int indent) {
		System.out.print(this.createIndent(indent));
	}
	public void printWithoutChildren(int indent) {
		this.printIndent(indent);
		System.out.println("<index>");

		for (int i = 0; i < this.audioIds.size(); i++) {
			this.printIndent(indent);
			System.out.println(this.audioIds.get(i));
		}

		this.printIndent(indent);
		System.out.println("</index>");
	}
	public void printSong(CengSong song, int indent) {
		this.printIndent(indent);

		System.out.println("<record>" + song.audioId() + "|" + song.genre() + "|" + song.songName() + "|" + song.artist() + "</record>");
	}
	public void print(int indent){
		this.printWithoutChildren(indent);
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
				ArrayList<CengSong> songs;
				PlaylistNodePrimaryLeaf currentLeaf;
				currentLeaf = (PlaylistNodePrimaryLeaf) child;
				songs = currentLeaf.getSongs();
				int length = songs.size();
				printIndent(indent);
				System.out.println("<data>");
				for(int i=0; i<length; i++){
					this.printSong(songs.get(i), indent);
				}
				printIndent(indent);
				System.out.println("</data>");
			}
		}
	}

	/**/
}
