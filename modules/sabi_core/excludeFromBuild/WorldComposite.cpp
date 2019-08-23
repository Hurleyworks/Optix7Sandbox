
using namespace sabi;

// ctor
WorldComposite::WorldComposite()
{

}

// dtor
WorldComposite::~WorldComposite()
{
	removeChildren();
}

void WorldComposite::addChild(RenderableNode node)
{
	if (node)
	{
		if (node && !hasChild(node))
		{
			// maybe remove node from current parent
			RenderableNode parent = node->getParent();
			if (parent)
				parent->removeChild(node);

			children.insert(std::make_pair(node->getID(), node));

			// set parent
			node->setParent(shared_from_this());
		}
		else
		{
			LOG(CRITICAL) << node->getName() << " is already in the scene";
		}
	}
	else
	{
		LOG(DBUG) << "FAILED TO ADD CHILD";
	}
}

RenderableNode WorldComposite::createClone(RenderableNode source)
{
	RenderableNode clone = WorldItem::create();

	clone->setDescription(source->description());
	clone->setSpacetime(source->getSpaceTime());
	clone->setMesh(source->getMesh());
	clone->setState(source->getState());

	return clone;
}

void WorldComposite::removeChild(RenderableNode node)
{
	auto it = children.find(node->getID());
	if (it != children.end())
	{
		
		RenderableNode node = it->second;

		// raise the deleteable flag
		node->getState().state |= PRenderableState::Deletable;

		// reset parent
		node->setParent(nullptr);
		
		children.erase(it);
	}
}

bool WorldComposite::hasChild(RenderableNode node) const
{
	auto it = children.find(node->getID());
	return it != children.end();
}

RenderableNode WorldComposite::findChild(ItemID itemID)
{
	auto it = children.find(itemID);
	return it == children.end() ? nullptr : it->second;
}

void WorldComposite::removeChildren()
{

	for (auto it : children)
	{
		RenderableNode node = it.second;
		if (!node) continue;

		// raise the deleteable flag
		node->getState().state |= PRenderableState::Deletable;

		// reset parent
		node->setParent(nullptr);
	}

	children.clear();

	// reset the unique id counter
	staticReset();
	
}

