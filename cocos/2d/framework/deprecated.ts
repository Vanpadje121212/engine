/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

/**
 * @packageDocumentation
 * @module ui
 */

import { removeProperty } from '../../core/utils';
import { UIComponent } from './ui-component';
import { UITransform } from './ui-transform';
import { UIRenderable } from './ui-renderable';
import { Canvas } from './canvas';
import { js } from '../../core/utils/js';
import { legacyCC } from '../../core/global-exports';

removeProperty(UIComponent.prototype, 'UIComponent',[
    {
        name: '_visibility',
    },
    {
        name: 'setVisibility',
    },
]);

/**
 * Alias of [[UITransform]]
 * @deprecated Since v1.2
 */
export { UITransform as UITransformComponent };
legacyCC.UITransformComponent = UITransform;
js.setClassAlias(UITransform, 'cc.UITransformComponent');

/**
 * Alias of [[UIRenderable]]
 * @deprecated Since v1.2
 */
export { UIRenderable as RenderComponent };
js.setClassAlias(UIRenderable, 'cc.RenderComponent');

/**
 * Alias of [[Canvas]]
 * @deprecated Since v1.2
 */
export { Canvas as CanvasComponent };
legacyCC.CanvasComponent = Canvas;
js.setClassAlias(Canvas, 'cc.CanvasComponent');