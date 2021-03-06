#ifndef COMPOSITEACTION_H
#define COMPOSITEACTION_H

/*
 * Copyright (C) 2007-2009 Sergio Pistone <sergio_pistone@yahoo.com.ar>
 * Copyright (C) 2010-2017 Mladen Milinkovic <max@smoothware.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "action.h"

#include <QString>
#include <QLinkedList>

namespace SubtitleComposer {
class CompositeAction : public Action
{
public:
	explicit CompositeAction(const QString &description, bool immediateExecution = true, bool delaySignals = true) :
		Action(description),
		m_actions(),
		m_immediateExecution(immediateExecution),
		m_delaySignals(delaySignals) {}

	virtual ~CompositeAction()
	{
		qDeleteAll(m_actions);
	}

	int count()
	{
		return m_actions.count();
	}

	/**
	 * @brief detachContainedAction
	 * @return contained action when there's only one, null otherwise
	 */
	Action * detachContainedAction()
	{
		if(m_actions.count() != 1)
			return 0;

		Action *action = m_actions.takeFirst();

		if(m_immediateExecution && m_delaySignals) {
			// NOTE: the action is in an "inconsistent" state: it has been
			// executed but the resulting signals have not been emitted yet
			action->internalEmitRedoSignals();
		}

		return action;
	}

	void appendAction(Action *action)
	{
		if(m_immediateExecution)
			action->internalRedo(!m_delaySignals);

		m_actions.append(action);
	}

protected:
	void compressActions()
	{
		if(m_actions.count() < 2)
			return;

		QLinkedList<Action *>::Iterator prevIt, it = m_actions.begin(), end = m_actions.end();
		for(prevIt = it, ++it; it != end; prevIt = it, ++it) {
			if((*it)->mergeWithPrevious(*prevIt)) {
				delete *prevIt;
				it = m_actions.erase(prevIt);
			}
		}
	}

	virtual void internalRedo()
	{
		if(executed() || !m_immediateExecution)
			for(QLinkedList<Action *>::ConstIterator it = m_actions.begin(), end = m_actions.end(); it != end; ++it)
				(*it)->internalRedo(!m_delaySignals);

		if(!executed())
			compressActions();
	}

	virtual void internalUndo()
	{
		if(m_actions.isEmpty())
			return;

		QLinkedList<Action *>::Iterator it = --m_actions.end();
		for(QLinkedList<Action *>::ConstIterator begin = m_actions.begin(); it != begin; --it)
			(*it)->internalUndo(!m_delaySignals);
		(*it)->internalUndo(!m_delaySignals);
	}

	virtual void internalEmitRedoSignals()
	{
		if(m_delaySignals)
			for(QLinkedList<Action *>::ConstIterator it = m_actions.begin(), end = m_actions.end(); it != end; ++it)
				(*it)->internalEmitRedoSignals();
	}

	virtual void internalEmitUndoSignals()
	{
		if(m_actions.isEmpty() || !m_delaySignals)
			return;

		QLinkedList<Action *>::Iterator it = --m_actions.end();
		for(QLinkedList<Action *>::ConstIterator begin = m_actions.begin(); it != begin; --it)
			(*it)->internalEmitUndoSignals();
		(*it)->internalEmitUndoSignals();
	}

protected:
	QLinkedList<Action *> m_actions;
	bool m_immediateExecution;
	bool m_delaySignals;
};
}

#endif
