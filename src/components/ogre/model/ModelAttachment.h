/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MODELATTACHMENT_H_
#define MODELATTACHMENT_H_

#include "components/ogre/NodeAttachment.h"
#include <sigc++/trackable.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <string>

namespace Atlas {
namespace Message {
class Element;
}
}

namespace Ember {
class AttributeObserver;
}

namespace Ember {
struct IGraphicalRepresentation;

class EmberEntity;
namespace OgreView {

struct INodeProvider;

namespace Model {

class ModelMount;

class ModelRepresentation;

class ModelFitting;

/**
 * @brief Attachment for a ModelRepresentation instance.
 *
 * Use this attachment when you have a ModelRepresentation you need to attach to an entity.
 */
class ModelAttachment : public NodeAttachment, public virtual sigc::trackable {
public:
	typedef std::unordered_map<std::string, std::unique_ptr<ModelFitting>> ModelFittingStore;

	ModelAttachment(EmberEntity& parentEntity, std::unique_ptr<ModelRepresentation> modelRepresentation, std::unique_ptr<INodeProvider> nodeProvider, const std::string& pose = "");

	~ModelAttachment() override;

	IGraphicalRepresentation* getGraphicalRepresentation() const override;

	void attachEntity(EmberEntity& entity) override;

	void updateScale() override;

	/**
	 * General method for turning on and off debug visualizations. Subclasses might support more types of visualizations than the ones defined here.
	 * @param visualization The type of visualization. Currently supports "OgreBBox".
	 * @param visualize Whether to visualize or not.
	 */
	void setVisualize(const std::string& visualization, bool visualize) override;

	/**
	 * @brief Gets whether a certain visualization is turned on or off.
	 * @param visualization The type of visualization. Currently supports "OgreBBox".
	 * @return true if visualization is turned on, else false
	 */
	bool getVisualize(const std::string& visualization) const override;

	void getOffsetForContainedNode(const IEntityAttachment& attachment, const WFMath::Point<3>& localPosition, WFMath::Vector<3>& offset) override;

	/**
	 * @brief Sets the position and orientation of the node.
	 * @param position The position.
	 * @param orientation The orientation.
	 * @param velocity The velocity.
	 */
	void setPosition(const WFMath::Point<3>& position, const WFMath::Quaternion& orientation, const WFMath::Vector<3>& velocity) override;


protected:

	typedef std::vector<std::unique_ptr<AttributeObserver>> AttributeObserverStore;

	/**
	 * @brief The Model representation which this attachment is connected to.
	 */
	std::unique_ptr<ModelRepresentation> mModelRepresentation;

	/**
	 * @brief An optional pose to use.
	 */
	const std::string mPose;

	/**
	 * @brief The model mount, which takes care of setting up and handling the rotation and orientation of the model.
	 * This also owns the scale node, which will be destroyed when the mount is destroyed.
	 * The model mount can be connected to either an Ogre::SceneNode or an Ogre::TagPoint. The former is the normal situation, while the latter is the situation if this attachment represents an attachment to a bone on an Ogre::Skeleton instance.
	 */
	std::unique_ptr<ModelMount> mModelMount;

	/**
	 * @brief The fittings, i.e. the connections to child entities which are connected to bones on the Model which this attachment belongs to.
	 */
	ModelFittingStore mFittings;

	/**
	 * @brief A collection of observers to observe entity attribute changes and attach and detach ModelFittings accordingly.
	 */
	AttributeObserverStore mFittingsObservers;

	/**
	 * @brief If true, all entity position and orientation updates will be ignored.
	 *
	 * This is set by the data in any pose that is specified. The main reason is that for some attachments we need to ignore the server for them to look alright.
	 */
	bool mIgnoreEntityData;

	/**
	 * @brief Sets up the ModelFittings and the observers of the attributes to which they are connected.
	 */
	void setupFittings();

	void entity_AttrChanged(const Atlas::Message::Element& attributeValue, const std::string& fittingName);

	void fittedEntity_BeingDeleted(EmberEntity* entity);

	/**
	 * @brief Detaches a previously attached fitting.
	 * @param entity The entity to attach.
	 */
	void detachFitting(EmberEntity& entity);


	/**
	 * @brief Creates a new ModelFitting instance.
	 * @param fittingName The name of the fitting.
	 * @param entityId The entity id of the attached entity.
	 *
	 * It's safe to call this before the attached entity has appeared in the world. In that case the attachment will be delayed until attachEntity() is later on called.
	 */
	void createFitting(const std::string& fittingName, const std::string& entityId);

	/**
	 * @brief Re-attach entities which previously have been attached.
	 * Call this when the Model is reloaded.
	 */
	void reattachEntities();

	void setVisible(bool visible) override;

	void model_Reloaded();
};

}
}

}

#endif /* MODELATTACHMENT_H_ */
